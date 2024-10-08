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
#include "NMEAMsgType.h"
#include "NMEALineWalker.h"

#include "StringTools.h"
#include "Logger.h"

NMEADBSMessage::NMEADBSMessage(const NMEATalker &talker) : NMEAMessage(talker) {
}

bool NMEADBSMessage::parse(NMEALineWalker &lineWalker) {
    if (!depthFeet.extract(lineWalker, talker, "DBS", "Depth Feet", true)) {
        return false;
    }

    if (!extractConstantWord(lineWalker, "DBS", "f")) {
        return false;
    }

    if (!depthMeters.extract(lineWalker, talker, "DBS", "Depth Meters", true)) {
        return false;
    }

    if (!extractConstantWord(lineWalker, "DBS", "M")) {
        return false;
    }

    if (!depthFathoms.extract(lineWalker, talker, "DBS", "Depth Fathoms", true)) {
        return false;
    }

    if (!extractConstantWord(lineWalker, "DBS", "F")) {
        return false;
    }

    return true;
}

NMEAMsgType::Value NMEADBSMessage::type() const {
    return NMEAMsgType::DBS;
}

void NMEADBSMessage::log() const {
    logger() << logDebugNMEA << talker << " DBS: Depth " << depthFeet << "', " << depthMeters
             << " m, " << depthFathoms << " ftm" << eol;
}

NMEADBSMessage *parseNMEADBSMessage(const NMEATalker &talker, NMEALineWalker &lineWalker,
                                    uint8_t *nmeaMessageBuffer) {
    NMEADBSMessage *message = new (nmeaMessageBuffer)NMEADBSMessage(talker);
    if (!message) {
        return nullptr;
    }

    if (!message->parse(lineWalker)) {
        // Since we use a static buffer and placement new for messages, we don't do a free here.
        return nullptr;
    }

    return message;
}
