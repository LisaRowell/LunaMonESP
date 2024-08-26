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

#include "NMEARSAMessage.h"
#include "NMEATalker.h"
#include "NMEAMsgType.h"
#include "NMEALineWalker.h"
#include "NMEATenthsInt16.h"
#include "NMEADataValid.h"

#include "Logger.h"

NMEARSAMessage::NMEARSAMessage(const NMEATalker &talker) : NMEAMessage(talker) {
}

bool NMEARSAMessage::parse(NMEALineWalker &lineWalker) {
    if (!starboardRudderSensorAngle.extract(lineWalker, talker, "RSA",
                                            "Starboard Rudder Sensor Angle", true)) {
        return false;
    }
    if (!starboardRudderSensorAngleValid.extract(lineWalker, talker, "RSA",
                                                 !starboardRudderSensorAngle.hasValue())) {
        return false;
    }

    if (!portRudderSensorAngle.extract(lineWalker, talker, "RSA", "Port Rudder Sensor Angle",
                                       true)) {
        return false;
    }
    if (!portRudderSensorAngleValid.extract(lineWalker, talker, "RSA",
                                            !portRudderSensorAngle.hasValue())) {
        return false;
    }

    return true;
}

NMEAMsgType::Value NMEARSAMessage::type() const {
    return NMEAMsgType::RSA;
}

void NMEARSAMessage::log() const {
    logger() << logDebugNMEA << talker << " RSA: Starboard rudder sensor angle ";
    if (starboardRudderSensorAngleValid) {
        logger() << starboardRudderSensorAngle;
    } else {
        logger() << "Invalid";
    }
    logger() << " Port rudder sensor angle ";
    if (portRudderSensorAngleValid) {
        logger() << portRudderSensorAngle;
    } else {
        logger() << "Invalid";
    }
    logger() << eol;
}

NMEARSAMessage *parseNMEARSAMessage(const NMEATalker &talker, NMEALineWalker &lineWalker,
                                    uint8_t *nmeaMessageBuffer) {
    NMEARSAMessage *message = new (nmeaMessageBuffer)NMEARSAMessage(talker);
    if (!message) {
        return nullptr;
    }

    if (!message->parse(lineWalker)) {
        // Since we use a static buffer and placement new for messages, we don't do a free here.
        return nullptr;
    }

    return message;
}
