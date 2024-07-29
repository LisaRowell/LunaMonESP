/*
 * This file is part of LunaMon (https://github.com/LisaRowell/LunaMonESP)
 * Copyright (C) 2023-2024 Lisa Rowell
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

#include "NMEADBSMessage.h"

#include "NMEATenthsUInt16.h"
#include "NMEATalker.h"
#include "NMEALine.h"
#include "NMEAMessageBuffer.h"

#include "StringTools.h"
#include "Logger.h"

NMEADBSMessage::NMEADBSMessage(NMEATalker &talker) : NMEAMessage(talker) {
}

bool NMEADBSMessage::parse(NMEALine &nmeaLine) {
    if (!depthFeet.extract(nmeaLine, talker, "DBS", "Depth Feet", true)) {
        return false;
    }

    if (!extractConstantWord(nmeaLine, "DBS", "f")) {
        return false;
    }

    if (!depthMeters.extract(nmeaLine, talker, "DBS", "Depth Meters", true)) {
        return false;
    }

    if (!extractConstantWord(nmeaLine, "DBS", "M")) {
        return false;
    }

    if (!depthFathoms.extract(nmeaLine, talker, "DBS", "Depth Fathoms", true)) {
        return false;
    }

    if (!extractConstantWord(nmeaLine, "DBS", "F")) {
        return false;
    }

    return true;
}

enum NMEAMsgType NMEADBSMessage::type() const {
    return NMEA_MSG_TYPE_DBS;
}

void NMEADBSMessage::log() const {
    logger() << logDebugNMEA << talker << " DBS: Depth " << depthFeet << "', " << depthMeters
             << " m, " << depthFathoms << " ftm" << eol;
}

NMEADBSMessage *parseNMEADBSMessage(NMEATalker &talker, NMEALine &nmeaLine) {
    NMEADBSMessage *message = new (nmeaMessageBuffer)NMEADBSMessage(talker);
    if (!message) {
        return nullptr;
    }

    if (!message->parse(nmeaLine)) {
        // Since we use a static buffer and placement new for messages, we don't do a free here.
        return nullptr;
    }

    return message;
}
