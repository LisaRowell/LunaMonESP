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

#include "NMEAGLLMessage.h"
#include "NMEAMsgType.h"
#include "NMEAMessage.h"
#include "NMEALine.h"
#include "NMEATalker.h"
#include "NMEAMsgType.h"
#include "NMEALatitude.h"
#include "NMEALongitude.h"
#include "NMEATime.h"
#include "NMEADataValid.h"
#include "NMEAFAAModeIndicator.h"
#include "NMEAMessageBuffer.h"

#include "Logger.h"

NMEAGLLMessage::NMEAGLLMessage(const NMEATalker &talker) : NMEAMessage(talker) {
}

bool NMEAGLLMessage::parse(NMEALine &nmeaLine) {
    if (!latitude.extract(nmeaLine, talker, "GLL")) {
        return false;
    }

    if (!longitude.extract(nmeaLine, talker, "GLL")) {
        return false;
    }

    if (!time.extract(nmeaLine, talker, "GLL")) {
        return false;
    }

    if (!dataValid.extract(nmeaLine, talker, "GLL")) {
        return false;
    }

    if (!faaModeIndicator.extract(nmeaLine, talker, "GLL")) {
        return false;
    }

    return true;
}

NMEAMsgType::Value NMEAGLLMessage::type() const {
    return NMEAMsgType::GLL;
}

void NMEAGLLMessage::log() const {
    logger() << logDebugNMEA << talker << " GLL: " << latitude << " " << longitude << " " << time
             << " " << dataValid;
    if (faaModeIndicator.hasValue()) {
        logger() << " " << faaModeIndicator;
    }
    logger() << eol;
}

NMEAGLLMessage *parseNMEAGLLMessage(const NMEATalker &talker, NMEALine &nmeaLine) {
    NMEAGLLMessage *message = new (nmeaMessageBuffer)NMEAGLLMessage(talker);
    if (!message) {
        return nullptr;
    }

    if (!message->parse(nmeaLine)) {
        // Since we use a static buffer and placement new for messages, we don't do a free here.
        return nullptr;
    }

    return message;
}
