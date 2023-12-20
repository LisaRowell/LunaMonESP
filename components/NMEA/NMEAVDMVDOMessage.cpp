/*
 * This file is part of LunaMon (https://github.com/LisaRowell/LunaMonESP)
 * Copyright (C) 2021-2023 Lisa Rowell
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

#include "NMEAVDMVDOMessage.h"
#include "NMEATalker.h"
#include "NMEALine.h"
#include "NMEAUInt8.h"
#include "NMEAUInt16.h"
#include "NMEARadioChannelCode.h"
#include "NMEAMsgType.h"
#include "NMEAMessageBuffer.h"

#include "Logger.h"

NMEAVDMVDOMessage::NMEAVDMVDOMessage(NMEATalker &talker, NMEAMsgType &msgType)
    : NMEAMessage(talker), msgType(msgType) {
}

bool NMEAVDMVDOMessage::parse(NMEALine &nmeaLine) {
    if (!nmeaLine.isEncapsulatedData()) {
        logger() << logWarnNMEA << talker << " " << msgTypeName()
                 << " message in unsupported, non-encapsulated format" << eol;
        return false;
    }

    if (!totalFragments.extract(nmeaLine, talker, msgTypeName(), "Total Fragments")) {
        return false;
    }

    if (!fragmentNumber.extract(nmeaLine, talker, msgTypeName(), "Fragment Number")) {
        return false;
    }

    if (!messageId.extract(nmeaLine, talker, msgTypeName(), "Message Id", true)) {
        return false;
    }

    if (!radioChannelCode.extract(nmeaLine, talker, msgTypeName())) {
        return false;
    }

    return true;
}

enum NMEAMsgType NMEAVDMVDOMessage::type() const {
    return msgType;
}

const char *NMEAVDMVDOMessage::msgTypeName() const {
    return nmeaMsgTypeName(msgType);
}

void NMEAVDMVDOMessage::log() const {
    logger() << logDebugNMEA << talker << " " << msgTypeName() << ": " << fragmentNumber << " of "
             << totalFragments << " Msg Id " << messageId << " Channel " << radioChannelCode << eol;
}

bool NMEAVDMVDOMessage::isOwnShip() const {
    return msgType == NMEA_MSG_TYPE_VDO;
}

NMEAVDMVDOMessage *parseNMEAVDMVDOMessage(NMEATalker &talker, NMEAMsgType &msgType,
                                          NMEALine &nmeaLine) {
    NMEAVDMVDOMessage *message = new (nmeaMessageBuffer)NMEAVDMVDOMessage(talker, msgType);
    if (!message) {
        return NULL;
    }

    if (!message->parse(nmeaLine)) {
        // Since we use a static buffer and placement new for messages, we don't do a free here.
        return NULL;
    }

    return message;
}
