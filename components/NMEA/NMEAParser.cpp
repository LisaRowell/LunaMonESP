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
#include "NMEAGGAMessage.h"
#include "NMEAGLLMessage.h"
#include "NMEAGSAMessage.h"
#include "NMEAGSTMessage.h"
#include "NMEAGSVMessage.h"
#include "NMEARMCMessage.h"
#include "NMEATXTMessage.h"
#include "NMEAVDMVDOMessage.h"
#include "NMEAVTGMessage.h"
#include "NMEAAISMsgType.h"
#include "NMEAVDMAidToNavigationMsg.h"
#include "NMEARadioChannelCode.h"
#include "NMEAUInt8.h"
#include "NMEAUInt32.h"
#include "NMEAMessageBuffer.h"

#include "Logger.h"

#include "etl/string.h"
#include "etl/string_view.h"
#include "etl/bit_stream.h"
#include "etl/endianness.h"

NMEAParser::NMEAParser() {
}

NMEAMessage *NMEAParser::parseLine(NMEALine &nmeaLine) {
    etl::string_view tagView;
    if (!nmeaLine.getWord(tagView)) {
        logger() << logWarnNMEA << "NMEA message missing tag" << eol;
        return NULL;
    }
    if (tagView.size() != 5) {
        logger() << logWarnNMEA << "Bad NMEA tag '" << tagView << "'" << eol;
        return NULL;
    }

    etl::string<2> talkerCode(tagView.begin(), 2);
    NMEATalker talker(talkerCode);

    etl::string<3> msgTypeStr(tagView.begin() + 2, 3);
    enum NMEAMsgType msgType = parseNMEAMsgType(msgTypeStr);

    if (!nmeaLine.isEncapsulatedData()) {
        return parseUnencapsulatedLine(talker, msgType, nmeaLine);
    } else {
        return parseEncapsulatedLine(talker, msgType, nmeaLine);
    }
}

NMEAMessage *NMEAParser::parseUnencapsulatedLine(NMEATalker &talker, enum NMEAMsgType msgType,
                                                 NMEALine &nmeaLine) {
    switch (msgType) {
        case NMEA_MSG_TYPE_DBK:
            return parseNMEADBKMessage(talker, nmeaLine);

        case NMEA_MSG_TYPE_DBS:
            return parseNMEADBSMessage(talker, nmeaLine);

        case NMEA_MSG_TYPE_DBT:
            return parseNMEADBTMessage(talker, nmeaLine);

        case NMEA_MSG_TYPE_GGA:
            return parseNMEAGGAMessage(talker, nmeaLine);

        case NMEA_MSG_TYPE_GLL:
            return parseNMEAGLLMessage(talker, nmeaLine);

        case NMEA_MSG_TYPE_GSA:
            return parseNMEAGSAMessage(talker, nmeaLine);

        case NMEA_MSG_TYPE_GST:
            return parseNMEAGSTMessage(talker, nmeaLine);

        case NMEA_MSG_TYPE_GSV:
            return parseNMEAGSVMessage(talker, nmeaLine);

        case NMEA_MSG_TYPE_RMC:
            return parseNMEARMCMessage(talker, nmeaLine);

        case NMEA_MSG_TYPE_TXT:
            return parseNMEATXTMessage(talker, nmeaLine);

        case NMEA_MSG_TYPE_VDM:
        case NMEA_MSG_TYPE_VDO:
            logger() << logWarnNMEA << "Unsupported unencapsulated " << nmeaMsgTypeName(msgType)
                     << " message from " << talker << eol;
            return NULL;

        case NMEA_MSG_TYPE_VTG:
            return parseNMEAVTGMessage(talker, nmeaLine);

        case NMEA_MSG_TYPE_UNKNOWN:
        default:
            logger() << logWarnNMEA << "Unknown NMEA message type (" << nmeaMsgTypeName(msgType)
                     << ") from " << talker << eol;
            return NULL;
    }
}

NMEAMessage *NMEAParser::parseEncapsulatedLine(NMEATalker &talker, enum NMEAMsgType msgType,
                                               NMEALine &nmeaLine) {
    NMEAUInt8 fragmentCount;
    if (!fragmentCount.extract(nmeaLine, talker, nmeaMsgTypeName(msgType), "Fragment Count")) {
        decapsulator.reset();
        return NULL;
    }
    if (fragmentCount == 0) {
        logger() << logWarnNMEA << "Encapsulated NMEA " << nmeaMsgTypeName(msgType)
                 << " message from " << talker << " with 0 fragment count" << eol;
        decapsulator.reset();
        return NULL;
    }

    NMEAUInt8 fragmentIndex;
    if (!fragmentIndex.extract(nmeaLine, talker, nmeaMsgTypeName(msgType), "Fragment Index")) {
        decapsulator.reset();
        return NULL;
    }
    if (fragmentIndex == 0) {
        logger() << logWarnNMEA << "Encapsulated NMEA " << nmeaMsgTypeName(msgType)
                 << " message from " << talker << " with 0 fragment index" << eol;
        decapsulator.reset();
        return NULL;
    }

    NMEAUInt32 messageID;
    if (!messageID.extract(nmeaLine, talker, nmeaMsgTypeName(msgType), "Message ID", true)) {
        decapsulator.reset();
        return NULL;
    }
    // We assume that all messages that are fragmented will contain a message id. This may need to
    // be revisited if it turns out that there are some talkers out there that don't send the id.
    if (fragmentCount > 1 && !messageID.hasValue()) {
        logger() << logWarnNMEA << "Encapsulated multi-fragment NMEA " << nmeaMsgTypeName(msgType)
                 << " message from " << talker << " without a message id" << eol;
        decapsulator.reset();
        return NULL;
    }
    const uint32_t messageIdOrZero = messageID.hasValue() ? messageID : 0;

    NMEARadioChannelCode radioChannelCode;
    if (!radioChannelCode.extract(nmeaLine, talker, nmeaMsgTypeName(msgType))) {
        decapsulator.reset();
        return NULL;
    }

    etl::string_view payloadView;
    if (!nmeaLine.getWord(payloadView)) {
        logger() << logWarnNMEA << "NMEA " << nmeaMsgTypeName(msgType) << " message from " << talker
                 << " missing payload" << eol;
        decapsulator.reset();
        return NULL;
    }

    NMEAUInt8 fillBits;
    if (!fillBits.extract(nmeaLine, talker, nmeaMsgTypeName(msgType), "Fill Bits", false, 5)) {
        decapsulator.reset();
        return NULL;
    }

    decapsulator.addFragment(talker, msgType, fragmentCount, fragmentIndex, messageIdOrZero,
                             payloadView, fillBits);

    if (decapsulator.isComplete()) {
        NMEAMessage *message = parseEncapsulatedMessage(talker, msgType);
        decapsulator.reset();
        return message;
    } else {
        return NULL;
    }
}

NMEAMessage *NMEAParser::parseEncapsulatedMessage(NMEATalker &talker, enum NMEAMsgType msgType) {
    switch (msgType) {
        case NMEA_MSG_TYPE_VDM:
            return parseVDMMessage(talker, msgType);

        case NMEA_MSG_TYPE_VDO:
            logger() << logNotifyNMEA << "Ignoring " << decapsulator.messageBitLength()
                     << " bit encapsulated NMEA VDO message from " << talker << eol;
            return NULL;

        default: 
            logger() << logWarnNMEA << "Ignoring unsupport encapsulated "
                     << nmeaMsgTypeName(msgType) << " message from " << talker << eol;
            return NULL;
    }
}

NMEAMessage *NMEAParser::parseVDMMessage(NMEATalker &talker, enum NMEAMsgType msgType) {
    etl::bit_stream_reader streamReader((void *)decapsulator.messageData().data(),
                                        decapsulator.messageByteLength(), etl::endian::big);
    const size_t messageSizeInBits = decapsulator.messageBitLength();

    if (messageSizeInBits < 6) {
        logger() << logWarnNMEA << "Encapsulated VDM message from " << talker
                 << " that too small to be valid (" << messageSizeInBits << " bits)" << eol;
        return NULL;
    }

    const uint8_t messageTypeCode = etl::read_unchecked<uint8_t>(streamReader, 6);
    const enum NMEAAISMsgType aisMsgType = parseNMEAAISMsgType(messageTypeCode);

    switch (aisMsgType) {
        case NMEA_AIS_MSG_TYPE_AID_TO_NAVIGATION_REPORT:
            return parseVDMAidToNavigationReportMessage(talker, streamReader, messageSizeInBits);
        default:
            logger() << logNotifyNMEA << "Ignoring " << talker << " "
                     << decapsulator.messageBitLength() << " bit NMEA VDM "
                     << nmeaAISMsgTypeName(aisMsgType) << " message" << eol;
            return NULL;
    }
}

NMEAMessage *NMEAParser::parseVDMAidToNavigationReportMessage(NMEATalker &talker,
                                                              etl::bit_stream_reader &streamReader,
                                                              size_t messageSizeInBits) {
    NMEAVDMAidToNavigationMsg *message = new (nmeaMessageBuffer)NMEAVDMAidToNavigationMsg(talker);
    if (!message) {
        return NULL;
    }

    if (!message->parse(streamReader, messageSizeInBits)) {
        // Since we use a static buffer and placement new for messages, we don't do a free here.
        return NULL;
    }

    return message;
}
