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

#include "NMEADBKMessage.h"

#include "NMEATenthsInt16.h"
#include "NMEATalker.h"
#include "NMEAMsgType.h"
#include "NMEALineWalker.h"

#include "StringTools.h"
#include "Logger.h"

NMEADBKMessage::NMEADBKMessage(const NMEATalker &talker) : NMEAMessage(talker) {
}

bool NMEADBKMessage::parse(NMEALineWalker &lineWalker) {
    if (!depthFeet.extract(lineWalker, talker, "DBK", "Depth Feet", true)) {
        return false;
    }

    if (!extractConstantWord(lineWalker, "DBK", "f")) {
        return false;
    }

    if (!depthMeters.extract(lineWalker, talker, "DBK", "Depth Meters", true)) {
        return false;
    }

    if (!extractConstantWord(lineWalker, "DBK", "M")) {
        return false;
    }

    if (!depthFathoms.extract(lineWalker, talker, "DBK", "Depth Fathoms", true)) {
        return false;
    }

    if (!extractConstantWord(lineWalker, "DBK", "F")) {
        return false;
    }

    return true;
}

NMEAMsgType::Value NMEADBKMessage::type() const {
    return NMEAMsgType::DBK;
}

void NMEADBKMessage::log() const {
    logger() << logDebugNMEA << talker << " DBK: Depth " << depthFeet << "', " << depthMeters
             << " m, " << depthFathoms << " ftm" << eol;
}

NMEADBKMessage *parseNMEADBKMessage(const NMEATalker &talker, NMEALineWalker &lineWalker,
                                    uint8_t *nmeaMessageBuffer) {
    NMEADBKMessage *message = new (nmeaMessageBuffer)NMEADBKMessage(talker);
    if (!message) {
        return nullptr;
    }

    if (!message->parse(lineWalker)) {
        // Since we use a static buffer and placement new for messages, we don't do a free here.
        return nullptr;
    }

    return message;
}
