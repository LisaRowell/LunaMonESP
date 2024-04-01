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

#include "NMEADBTMessage.h"

#include "NMEATenthsUInt16.h"
#include "NMEATalker.h"
#include "NMEALine.h"
#include "NMEAMessageBuffer.h"

#include "StringTools.h"
#include "Logger.h"

NMEADBTMessage::NMEADBTMessage(NMEATalker &talker) : NMEAMessage(talker) {
}

bool NMEADBTMessage::parse(NMEALine &nmeaLine) {
    if (!depthFeet.extract(nmeaLine, talker, "DBT", "Depth Feet", true)) {
        return false;
    }

    if (!extractConstantWord(nmeaLine, "DBT", "f")) {
        return false;
    }

    if (!depthMeters.extract(nmeaLine, talker, "DBT", "Depth Meters", true)) {
        return false;
    }

    if (!extractConstantWord(nmeaLine, "DBT", "M")) {
        return false;
    }

    if (!depthFathoms.extract(nmeaLine, talker, "DBT", "Depth Fathoms", true)) {
        return false;
    }

    if (!extractConstantWord(nmeaLine, "DBT", "F")) {
        return false;
    }

    return true;
}

enum NMEAMsgType NMEADBTMessage::type() const {
    return NMEA_MSG_TYPE_DBT;
}

void NMEADBTMessage::log() const {
    logger() << logDebugNMEA << talker << " DBT: Depth " << depthFeet << "', " << depthMeters
             << " m, " << depthFathoms << " ftm" << eol;
}

NMEADBTMessage *parseNMEADBTMessage(NMEATalker &talker, NMEALine &nmeaLine) {
    NMEADBTMessage *message = new (nmeaMessageBuffer)NMEADBTMessage(talker);
    if (!message) {
        return NULL;
    }

    if (!message->parse(nmeaLine)) {
        // Since we use a static buffer and placement new for messages, we don't do a free here.
        return NULL;
    }

    return message;
}
