/*
 * This file is part of LunaMon (https://github.com/LisaRowell/LunaMonESP)
 * Copyright (C) 2021-2024 Lisa Rowell
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

#include "NMEAParser.h"

#include "NMEALine.h"
#include "NMEATalker.h"
#include "NMEAMsgType.h"
#include "NMEAMessage.h"
#include "NMEADBKMessage.h"
#include "NMEADBSMessage.h"
#include "NMEADBTMessage.h"
#include "NMEADPTMessage.h"
#include "NMEAGGAMessage.h"
#include "NMEAGLLMessage.h"
#include "NMEAGSAMessage.h"
#include "NMEAGSTMessage.h"
#include "NMEAGSVMessage.h"
#include "NMEAMTWMessage.h"
#include "NMEAMWVMessage.h"
#include "NMEARMCMessage.h"
#include "NMEATXTMessage.h"
#include "NMEAVDMMessage.h"
#include "NMEAVDOMessage.h"
#include "NMEAVHWMessage.h"
#include "NMEAVTGMessage.h"
#include "NMEARadioChannelCode.h"
#include "NMEAUInt8.h"
#include "NMEAUInt32.h"
#include "NMEAMessageBuffer.h"

#include "AISContacts.h"

#include "Logger.h"

#include "etl/string.h"
#include "etl/string_view.h"
#include "etl/bit_stream.h"
#include "etl/endianness.h"

NMEAParser::NMEAParser(AISContacts &aisContacts) : aisContacts(aisContacts) {
}

NMEAMessage *NMEAParser::parseLine(NMEALine &nmeaLine) {
    etl::string_view tagView;
    if (!nmeaLine.getWord(tagView)) {
        logger() << logWarnNMEA << "NMEA message missing tag" << eol;
        return nullptr;
    }
    if (tagView.size() != 5) {
        logger() << logWarnNMEA << "Bad NMEA tag '" << tagView << "'" << eol;
        return nullptr;
    }

    etl::string<2> talkerCode(tagView.begin(), 2);
    NMEATalker talker(talkerCode);

    etl::string<3> msgTypeStr(tagView.begin() + 2, 3);
    NMEAMsgType msgType(msgTypeStr);
    if (msgType == NMEAMsgType::UNKNOWN) {
        logger() << logWarnNMEA << "NMEA message with unknown type (" << msgTypeStr << ")"
                 << " from " << talker << ". Ignored." << eol;
        return nullptr;
    }

    if (!nmeaLine.isEncapsulatedData()) {
        return parseUnencapsulatedLine(talker, msgType, nmeaLine);
    } else {
        return parseEncapsulatedLine(talker, msgType, nmeaLine);
    }
}

NMEAMessage *NMEAParser::parseUnencapsulatedLine(const NMEATalker &talker,
                                                 const NMEAMsgType &msgType, NMEALine &nmeaLine) {
    switch (msgType) {
        case NMEAMsgType::DBK:
            return parseNMEADBKMessage(talker, nmeaLine);

        case NMEAMsgType::DBS:
            return parseNMEADBSMessage(talker, nmeaLine);

        case NMEAMsgType::DBT:
            return parseNMEADBTMessage(talker, nmeaLine);

        case NMEAMsgType::DPT:
            return parseNMEADPTMessage(talker, nmeaLine);

        case NMEAMsgType::GGA:
            return parseNMEAGGAMessage(talker, nmeaLine);

        case NMEAMsgType::GLL:
            return parseNMEAGLLMessage(talker, nmeaLine);

        case NMEAMsgType::GSA:
            return parseNMEAGSAMessage(talker, nmeaLine);

        case NMEAMsgType::GST:
            return parseNMEAGSTMessage(talker, nmeaLine);

        case NMEAMsgType::GSV:
            return parseNMEAGSVMessage(talker, nmeaLine);

        case NMEAMsgType::MTW:
            return parseNMEAMTWMessage(talker, nmeaLine);

        case NMEAMsgType::MWV:
            return parseNMEAMWVMessage(talker, nmeaLine);

        case NMEAMsgType::RMC:
            return parseNMEARMCMessage(talker, nmeaLine);

        case NMEAMsgType::TXT:
            return parseNMEATXTMessage(talker, nmeaLine);

        case NMEAMsgType::VDM:
        case NMEAMsgType::VDO:
            logger() << logWarnNMEA << "Unsupported unencapsulated " << msgType << " message from "
                     << talker << eol;
            return nullptr;

        case NMEAMsgType::VHW:
            return parseNMEAVHWMessage(talker, nmeaLine);

        case NMEAMsgType::VTG:
            return parseNMEAVTGMessage(talker, nmeaLine);

        case NMEAMsgType::UNKNOWN:
        default:
            logger() << logWarnNMEA << "Unhandled NMEA message type (" << msgType << ") from "
                     << talker << eol;
            return nullptr;
    }
}

NMEAMessage *NMEAParser::parseEncapsulatedLine(const NMEATalker &talker,
                                               const NMEAMsgType &msgType,
                                               NMEALine &nmeaLine) {
    NMEAUInt8 fragmentCount;
    if (!fragmentCount.extract(nmeaLine, talker, msgType.name(), "Fragment Count")) {
        decapsulator.reset();
        return nullptr;
    }
    if (fragmentCount == 0) {
        logger() << logWarnNMEA << "Encapsulated NMEA " << msgType << " message from " << talker
                 << " with 0 fragment count" << eol;
        decapsulator.reset();
        return nullptr;
    }

    NMEAUInt8 fragmentIndex;
    if (!fragmentIndex.extract(nmeaLine, talker, msgType.name(), "Fragment Index")) {
        decapsulator.reset();
        return nullptr;
    }
    if (fragmentIndex == 0) {
        logger() << logWarnNMEA << "Encapsulated NMEA " << msgType << " message from " << talker
                 << " with 0 fragment index" << eol;
        decapsulator.reset();
        return nullptr;
    }

    NMEAUInt32 messageID;
    if (!messageID.extract(nmeaLine, talker, msgType.name(), "Message ID", true)) {
        decapsulator.reset();
        return nullptr;
    }
    // We assume that all messages that are fragmented will contain a message id. This may need to
    // be revisited if it turns out that there are some talkers out there that don't send the id.
    if (fragmentCount > 1 && !messageID.hasValue()) {
        logger() << logWarnNMEA << "Encapsulated multi-fragment NMEA " << msgType
                 << " message from " << talker << " without a message id" << eol;
        decapsulator.reset();
        return nullptr;
    }
    const uint32_t messageIdOrZero = messageID.hasValue() ? messageID : 0;

    NMEARadioChannelCode radioChannelCode;
    if (!radioChannelCode.extract(nmeaLine, talker, msgType)) {
        decapsulator.reset();
        return nullptr;
    }

    etl::string_view payloadView;
    if (!nmeaLine.getWord(payloadView)) {
        logger() << logWarnNMEA << "NMEA " << msgType << " message from " << talker
                 << " missing payload" << eol;
        decapsulator.reset();
        return nullptr;
    }

    NMEAUInt8 fillBits;
    if (!fillBits.extract(nmeaLine, talker, msgType.name(), "Fill Bits", false, 5)) {
        decapsulator.reset();
        return nullptr;
    }

    decapsulator.addFragment(talker, msgType, fragmentCount, fragmentIndex, messageIdOrZero,
                             payloadView, fillBits);

    if (decapsulator.isComplete()) {
        NMEAMessage *message = parseEncapsulatedMessage(talker, msgType);
        decapsulator.reset();
        return message;
    } else {
        return nullptr;
    }
}

NMEAMessage *NMEAParser::parseEncapsulatedMessage(const NMEATalker &talker,
                                                  const NMEAMsgType &msgType) {
    etl::bit_stream_reader streamReader((void *)decapsulator.messageData().data(),
                                        decapsulator.messageByteLength(), etl::endian::big);
    const size_t messageSizeInBits = decapsulator.messageBitLength();

    switch (msgType) {
        case NMEAMsgType::VDM:
            return parseVDMMessage(talker, streamReader, messageSizeInBits, aisContacts);

        case NMEAMsgType::VDO:
            return parseVDOMessage(talker, streamReader, messageSizeInBits, aisContacts);

        default: 
            logger() << logWarnNMEA << "Ignoring unsupport encapsulated " << msgType
                     << " message from " << talker << eol;
            return nullptr;
    }
}
