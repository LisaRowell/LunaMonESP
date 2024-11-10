/*
 * This file is part of LunaMon (https://github.com/LisaRowell/LunaMonESP)
 * Copyright (C) 2021-2024 Lisa Rowell
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

#include "NMEARMCMessage.h"
#include "NMEATime.h"
#include "NMEADataValid.h"
#include "NMEALatitude.h"
#include "NMEALongitude.h"
#include "NMEATenthsUInt16.h"
#include "NMEADate.h"
#include "NMEAHeadingOffset.h"
#include "NMEAFAAModeIndicator.h"
#include "NMEATalker.h"
#include "NMEAMsgType.h"
#include "NMEALineWalker.h"

#include "Logger.h"

NMEARMCMessage::NMEARMCMessage(const NMEATalker &talker) : NMEAMessage(talker) {
}

bool NMEARMCMessage::parse(NMEALineWalker &lineWalker) {
    if (!time.extract(lineWalker, talker, "RMC", true)) {
        return false;
    }

    if (!dataValid.extract(lineWalker, talker, "RMC")) {
        return false;
    }

    if (!latitude.extract(lineWalker, talker, "RMC", true)) {
        return false;
    }

    if (!longitude.extract(lineWalker, talker, "RMC", true)) {
        return false;
    }

    if (!speedOverGround.extract(lineWalker, talker, "RMC", "Speed Over Ground", true)) {
        return false;
    }

    if (!trackMadeGood.extract(lineWalker, talker, "RMC", "Track Made Good", true)) {
        return false;
    }

    if (!date.extract(lineWalker, talker, "RMC")) {
        return false;
    }

    if (!magneticVariation.extract(lineWalker, talker, "RMC", "Magnetic Variation")) {
        return false;
    }

    if (!faaModeIndicator.extract(lineWalker, talker, "RMC")) {
        return false;
    }

    return true;
}

NMEAMsgType::Value NMEARMCMessage::type() const {
    return NMEAMsgType::RMC;
}

void NMEARMCMessage::log() const {
    logger() << logDebugNMEA << talker << " RMC: " << time << " " << dataValid << " " << latitude
             << " " << longitude << " " << speedOverGround << "kn " << trackMadeGood << " " << date
             << " " << magneticVariation;
    if (faaModeIndicator.hasValue()) {
        logger() << " " << faaModeIndicator;
    }
    logger() << eol;
}

NMEARMCMessage *parseNMEARMCMessage(const NMEATalker &talker, NMEALineWalker &lineWalker,
                                    uint8_t *nmeaMessageBuffer) {
    NMEARMCMessage *message = new (nmeaMessageBuffer)NMEARMCMessage(talker);
    if (!message) {
        return nullptr;
    }

    if (!message->parse(lineWalker)) {
        // Since we use a static buffer and placement new for messages, we don't do a free here.
        return nullptr;
    }

    return message;
}
