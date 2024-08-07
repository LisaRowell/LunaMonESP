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

#include "STALKSource.h"

#include "NMEALine.h"
#include "StatCounter.h"
#include "StatsManager.h"
#include "DataModelNode.h"
#include "DataModelUInt32Leaf.h"
#include "Logger.h"

#include <stdint.h>

STALKSource::STALKSource(const char *name, DataModelNode &interfaceNode,
                         StatsManager &statsManager)
    : messagesLeaf("messages", &interfaceNode),
      messageRateLeaf("messageRate", &interfaceNode),
      illformedMessages(0),
      illformedMessagesLeaf("illformedMessages", &interfaceNode) {
    statsManager.addStatsHolder(*this);
}

void STALKSource::handleLine(NMEALine &inputLine) {
    logger() << logDebugSTALK << inputLine << eol;

    if (!parseLine(inputLine)) {
        illformedMessages++;
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
bool STALKSource::parseLine(NMEALine &nmeaLine) {
    etl::string_view tagView;
    if (!nmeaLine.getWord(tagView)) {
        logger() << logWarnSTALK << "Illformed $STALK message on STALK interface: " << nmeaLine
                 << eol;
        return false;
    }

    if (tagView != "STALK") {
        logger() << logWarnSTALK << "Non $STALK message (" << tagView << ") on STALK interface"
                 << eol;
        return false;
    }

    return true;
}

void STALKSource::exportStats(uint32_t msElapsed) {
    messagesCounter.update(messagesLeaf, messageRateLeaf, msElapsed);
    illformedMessagesLeaf = illformedMessages;
}

bool STALKSource::lastMessageIllformed() const {
    return _lastMessageIllformed;
}
