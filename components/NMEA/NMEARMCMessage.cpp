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
#include "NMEAMagneticVariation.h"
#include "NMEAFAAModeIndicator.h"
#include "NMEATalker.h"
#include "NMEALine.h"
#include "NMEAMessageBuffer.h"

#include "Logger.h"

NMEARMCMessage::NMEARMCMessage(NMEATalker &talker) : NMEAMessage(talker) {
}

bool NMEARMCMessage::parse(NMEALine &nmeaLine) {
    if (!time.extract(nmeaLine, talker, "RMC")) {
        return false;
    }

    if (!dataValid.extract(nmeaLine, talker, "RMC")) {
        return false;
    }

    if (!latitude.extract(nmeaLine, talker, "RMC")) {
        return false;
    }

    if (!longitude.extract(nmeaLine, talker, "RMC")) {
        return false;
    }

    if (!speedOverGround.extract(nmeaLine, talker, "RMC", "Speed Over Ground")) {
        return false;
    }

    if (!trackMadeGood.extract(nmeaLine, talker, "RMC", "Track Made Good", true)) {
        return false;
    }

    if (!date.extract(nmeaLine, talker, "RMC")) {
        return false;
    }

    if (!magneticVariation.extract(nmeaLine, talker, "RMC")) {
        return false;
    }

    if (!faaModeIndicator.extract(nmeaLine, talker, "RMC")) {
        return false;
    }

    return true;
}

enum NMEAMsgType NMEARMCMessage::type() const {
    return NMEA_MSG_TYPE_RMC;
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

NMEARMCMessage *parseNMEARMCMessage(NMEATalker &talker, NMEALine &nmeaLine) {
    NMEARMCMessage *message = new (nmeaMessageBuffer)NMEARMCMessage(talker);
    if (!message) {
        return nullptr;
    }

    if (!message->parse(nmeaLine)) {
        // Since we use a static buffer and placement new for messages, we don't do a free here.
        return nullptr;
    }

    return message;
}
