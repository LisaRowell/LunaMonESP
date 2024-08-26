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

#include "NMEAMWVMessage.h"
#include "NMEATalker.h"
#include "NMEAMsgType.h"
#include "NMEALineWalker.h"
#include "NMEATenthsUInt16.h"
#include "NMEARelativeIndicator.h"
#include "NMEASpeedUnits.h"

#include "Logger.h"

NMEAMWVMessage::NMEAMWVMessage(const NMEATalker &talker) : NMEAMessage(talker) {
}

bool NMEAMWVMessage::parse(NMEALineWalker &lineWalker) {
    if (!windAngle.extract(lineWalker, talker, "MWV", "Wind Angle")) {
        return false;
    }

    if (!relativeIndicator.extract(lineWalker, talker, "MWV")) {
        return false;
    }

    if (!windSpeed.extract(lineWalker, talker, "MWV", "Wind Speed")) {
        return false;
    }

    if (!windSpeedUnits.extract(lineWalker, talker, "MWV")) {
        return false;
    }

    if (!dataValid.extract(lineWalker, talker, "MWV")) {
        return false;
    }

    return true;
}

NMEAMsgType::Value NMEAMWVMessage::type() const {
    return NMEAMsgType::MWV;
}

void NMEAMWVMessage::log() const {
    logger() << logDebugNMEA << talker << " MWV: " << relativeIndicator << " wind speed "
             << windSpeed << " " << windSpeedUnits << " angle " << windAngle << " " << dataValid
             << eol;
}

NMEAMWVMessage *parseNMEAMWVMessage(const NMEATalker &talker, NMEALineWalker &lineWalker,
                                    uint8_t *nmeaMessageBuffer) {
    NMEAMWVMessage *message = new (nmeaMessageBuffer)NMEAMWVMessage(talker);
    if (!message) {
        return nullptr;
    }

    if (!message->parse(lineWalker)) {
        // Since we use a static buffer and placement new for messages, we don't do a free here.
        return nullptr;
    }

    return message;
}
