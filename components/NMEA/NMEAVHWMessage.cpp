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

#include "NMEAVHWMessage.h"
#include "NMEATalker.h"
#include "NMEAMsgType.h"
#include "NMEALineWalker.h"
#include "NMEATenthsUInt16.h"
#include "NMEARelativeIndicator.h"
#include "NMEASpeedUnits.h"

#include "Logger.h"

NMEAVHWMessage::NMEAVHWMessage(const NMEATalker &talker) : NMEAMessage(talker) {
}

bool NMEAVHWMessage::parse(NMEALineWalker &lineWalker) {
    if (!waterHeadingTrue.extract(lineWalker, talker, "VHW", "Water Heading True", true)) {
        return false;
    }
    if (!extractConstantWord(lineWalker, "VHW", "T", !waterHeadingTrue.hasValue())) {
        return false;
    }

    if (!waterHeadingMagnetic.extract(lineWalker, talker, "VHW", "Water Heading Magnetic", true)) {
        return false;
    }
    if (!extractConstantWord(lineWalker, "VHW", "M", !waterHeadingMagnetic.hasValue())) {
        return false;
    }

    if (!waterSpeedKnots.extract(lineWalker, talker, "VHW", "Water Speed Knots")) {
        return false;
    }
    if (!extractConstantWord(lineWalker, "VHW", "N")) {
        return false;
    }

    if (!waterSpeedKMPH.extract(lineWalker, talker, "VHW", "Water Speed km/h")) {
        return false;
    }
    if (!extractConstantWord(lineWalker, "VHW", "K")) {
        return false;
    }

    return true;
}

NMEAMsgType::Value NMEAVHWMessage::type() const {
    return NMEAMsgType::VHW;
}

void NMEAVHWMessage::log() const {
    logger() << logDebugNMEA << talker << " VHW: Speed " << waterSpeedKnots << "kn "
             << waterSpeedKMPH << "km/h";
    if (waterHeadingTrue.hasValue() || waterHeadingMagnetic.hasValue()) {
        logger() << " Heading";
        if (waterHeadingTrue.hasValue()) {
            logger() << " " << waterHeadingTrue << " True";
        }
        if (waterHeadingMagnetic.hasValue()) {
            logger() << " " << waterHeadingMagnetic << " Magnetic";
        }
    }
    logger() << eol;
}

NMEAVHWMessage *parseNMEAVHWMessage(const NMEATalker &talker, NMEALineWalker &lineWalker,
                                    uint8_t *nmeaMessageBuffer) {
    NMEAVHWMessage *message = new (nmeaMessageBuffer)NMEAVHWMessage(talker);
    if (!message) {
        return nullptr;
    }

    if (!message->parse(lineWalker)) {
        // Since we use a static buffer and placement new for messages, we don't do a free here.
        return nullptr;
    }

    return message;
}
