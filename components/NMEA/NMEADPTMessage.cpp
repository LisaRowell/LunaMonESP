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

#include "NMEADPTMessage.h"

#include "NMEATenthsUInt16.h"
#include "NMEATalker.h"
#include "NMEAMsgType.h"
#include "NMEALine.h"
#include "NMEAMessageBuffer.h"

#include "StringTools.h"
#include "Logger.h"

NMEADPTMessage::NMEADPTMessage(const NMEATalker &talker) : NMEAMessage(talker) {
}

bool NMEADPTMessage::parse(NMEALine &nmeaLine) {
    if (!depthBelowTransducerMeters.extract(nmeaLine, talker, "DPT", "Depth")) {
        return false;
    }

    if (!transducerOffsetMeters.extract(nmeaLine, talker, "DPT", "Transducer Offset")) {
        return false;
    }

    // NMEA 3.0 added a max range scale field, early messages only had two fields. Untested...
    if (!nmeaLine.atEndOfLine()) {
        if (!maxRangeScaleMeters.extract(nmeaLine, talker, "DPT", "Max Range Scale"), true) {
            return false;
        }
    }

    return true;
}

NMEAMsgType::Value NMEADPTMessage::type() const {
    return NMEAMsgType::DPT;
}

void NMEADPTMessage::log() const {
    logger() << logDebugNMEA << talker << " DPT: Depth below transducer "
             << depthBelowTransducerMeters << "m Transducer offset " << transducerOffsetMeters
             << "m";
    if (maxRangeScaleMeters.hasValue()) {
        logger() << " Max range scale " << maxRangeScaleMeters << "m";
    }
    logger() << eol;
}

NMEADPTMessage *parseNMEADPTMessage(const NMEATalker &talker, NMEALine &nmeaLine) {
    NMEADPTMessage *message = new (nmeaMessageBuffer)NMEADPTMessage(talker);
    if (!message) {
        return nullptr;
    }

    if (!message->parse(nmeaLine)) {
        // Since we use a static buffer and placement new for messages, we don't do a free here.
        return nullptr;
    }

    return message;
}
