/*
 * This file is part of LunaMon (https://github.com/LisaRowell/LunaMonESP)
 * Copyright (C) 2024 Lisa Rowell
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "STALKInterface.h"
#include "SeaTalk.h"
#include "SeaTalkInterface.h"
#include "Interface.h"
#include "NMEALine.h"
#include "NMEALineWalker.h"
#include "StatCounter.h"
#include "StatsManager.h"
#include "DataModelNode.h"
#include "DataModelUInt32Leaf.h"
#include "Logger.h"

#include "etl/string.h"
#include "etl/string_view.h"
#include "etl/to_string.h"
#include "etl/vector.h"
#include "etl/to_arithmetic.h"

#include <stddef.h>
#include <stdint.h>

STALKInterface::STALKInterface(Interface &interface, InstrumentData &instrumentData,
                               StatsManager &statsManager)
    : SeaTalkInterface(interface, instrumentData, statsManager),
      interface(interface),
      messagesCounter(),
      illformedMessages(0),
      _lastMessageIllformed(false),
      stalkNode("stalk", &interface.interfaceNode()),
      messagesLeaf("messages", &stalkNode),
      messageRateLeaf("messageRate", &stalkNode),
      illformedMessagesLeaf("illformedMessages", &stalkNode) {
    // Note that we don't add STALKInterfaces to the stats manager as a stats holder because they
    // are derived from SeatalkInterfaces, which also are StatsHolders. Instead we override the
    // exportStats method and call the parent classes version ourselves.

    addLineHandler(*this);
}

void STALKInterface::handleLine(const NMEALine &inputLine) {
    logger() << logDebugSTALK << inputLine << eol;

    if (!parseLine(inputLine)) {
        // We keep track of whether or not the last message was a valid $STALK message so that
        // the work around for Digitial Yachts' ST-NMEA (ISO) device not saving its config can be
        // applied.
        _lastMessageIllformed = true;
    } else {
        _lastMessageIllformed = false;
    }
}

// $TALK lines are essentially a NMEA 0183 message with an "ST" talker and "ALK" message code
// containing a SeaTalk message encoded as comma separated ASCII versions of the bytes in the
// message with the ninth bit discarded.
// Returns false iff the NMEA message wasn't a $STALK message at all.
bool STALKInterface::parseLine(const NMEALine &inputLine) {
    NMEALineWalker walker(inputLine);

    // At this point we can assume the line has either a leading '$' or '!' and has a valid
    // checksum.
    walker.skipChar();
    if (walker.isEncapsulatedData()) {
        logger() << logWarnSTALK << "Encapsulated NMEA message on $STALK interface:" << inputLine
                 << eol;
        return false;
    }
    walker.stripChecksum();

    etl::string_view tagView;
    if (!walker.getWord(tagView)) {
        logger() << logWarnSTALK << "Illformed $STALK message on STALK interface: " << inputLine
                 << eol;
        illformedMessages++;
        return false;
    }

    if (tagView == "STALK") {
        parseDatagramMessage(inputLine, walker);
    } else if (tagView == "PDGY") {
        parsePropritoryMessage(inputLine);
    } else {
        logger() << logWarnSTALK << "Non $STALK message (" << tagView << ") on STALK interface"
                 << eol;
        illformedMessages++;
        return false;
    }

    return true;
}

void STALKInterface::parseDatagramMessage(const NMEALine &nmeaLine, NMEALineWalker &walker) {
    etl::vector<uint16_t, MAX_SEA_TALK_LINE_LENGTH> seaTalkDatagram;
    bool onCommandCharacter = true;
    while (!walker.atEndOfLine()) {
        etl::string_view msgByteView;
        if (!walker.getWord(msgByteView)) {
            logger() << logWarnSTALK << "Illformed $STALK message on STALK interface: " << nmeaLine
                     << eol;
            illformedMessages++;
            return;
        }

        int result = etl::to_arithmetic<int>(msgByteView, etl::radix::hex);
        if (result < 0 || result > 0xff) {
            logger() << logWarnSTALK << "$STALK message with bad byte encoding (" << msgByteView
                     << "): " << nmeaLine << eol;
            illformedMessages++;
            return;
        }

        if (seaTalkDatagram.full()) {
            logger() << logWarnSTALK << "$STALK message longer than max allowed SeaTalk message: "
                    << nmeaLine << eol;
            illformedMessages++;
            return;
        }

        if (onCommandCharacter) {
            // The start of a SeaTalk datagram is denoted by a command character with its MSB set
            seaTalkDatagram.push_back(result | 0x100);
            onCommandCharacter = false;
        } else {
            seaTalkDatagram.push_back(result);
        }
    }

    SeaTalkInterface::processBuffer(seaTalkDatagram.data(), seaTalkDatagram.size());
    messagesCounter++;
}

void STALKInterface::parsePropritoryMessage(const NMEALine &nmeaLine) {
    logger() << logDebugSTALK << "Ignoring propritory message: " << nmeaLine << eol;
    messagesCounter++;
}

// Not called on the interface task!
void STALKInterface::sendCommand(const SeaTalkLine &seaTalkLine) {
    NMEALine nmeaLine;
    nmeaLine.append("$STALK");

    // Todo add an c++ iterator to seaTalkLine
    etl::format_spec byteFormat;
    byteFormat.hex().upper_case(true).width(2).fill('0');
    for (int pos = 0; pos < seaTalkLine.length(); pos++) {
        uint8_t datagramByte = seaTalkLine[pos];
        etl::string<4> datagramByteString;
        etl::to_string(datagramByte, datagramByteString, byteFormat);
        nmeaLine.appendWord(datagramByteString);
    }

    nmeaLine.appendChecksum();
    nmeaLine.append("\r\n");

    taskLogger() << logDebugSTALK << "Sending STALK command: " << nmeaLine << eol;

    size_t bytesWritten = interface.send(nmeaLine.contents());
    if (bytesWritten != nmeaLine.contents().length()) {
        taskLogger() << logWarnSTALK << "Write failure sended SeaTalk command to interface "
                     << interface.name() << eol;
    }
}

void STALKInterface::exportStats(uint32_t msElapsed) {
    messagesCounter.update(messagesLeaf, messageRateLeaf, msElapsed);
    illformedMessagesLeaf = illformedMessages;

    SeaTalkInterface::exportStats(msElapsed);
}

bool STALKInterface::lastMessageIllformed() const {
    return _lastMessageIllformed;
}
