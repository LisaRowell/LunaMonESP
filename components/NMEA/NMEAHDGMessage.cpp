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

#include "NMEAHDGMessage.h"
#include "NMEATalker.h"
#include "NMEAMsgType.h"
#include "NMEALineWalker.h"
#include "NMEATenthsUInt16.h"
#include "NMEAHeadingOffset.h"

#include "Logger.h"

NMEAHDGMessage::NMEAHDGMessage(const NMEATalker &talker) : NMEAMessage(talker) {
}

bool NMEAHDGMessage::parse(NMEALineWalker &lineWalker) {
    if (!magneticSensorHeading.extract(lineWalker, talker, "HDG", "Magnetic Sensor Heading")) {
        return false;
    }

    if (!magneticDeviation.extract(lineWalker, talker, "HDG", "Magnetic Deviation")) {
        return false;
    }

    if (!magneticVariation.extract(lineWalker, talker, "HDG", "Magnetic Variation")) {
        return false;
    }

    return true;
}

NMEAMsgType::Value NMEAHDGMessage::type() const {
    return NMEAMsgType::HDG;
}

void NMEAHDGMessage::log() const {
    logger() << logDebugNMEA << talker << " HDG: Magnetic sensor heading " << magneticSensorHeading
             << " Deviation " << magneticDeviation << " Variation " << magneticVariation << eol;
}

NMEAHDGMessage *parseNMEAHDGMessage(const NMEATalker &talker, NMEALineWalker &lineWalker,
                                    uint8_t *nmeaMessageBuffer) {
    NMEAHDGMessage *message = new (nmeaMessageBuffer)NMEAHDGMessage(talker);
    if (!message) {
        return nullptr;
    }

    if (!message->parse(lineWalker)) {
        // Since we use a static buffer and placement new for messages, we don't do a free here.
        return nullptr;
    }

    return message;
}
