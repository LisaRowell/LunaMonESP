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

#include "NMEAMTWMessage.h"
#include "NMEATalker.h"
#include "NMEAMsgType.h"
#include "NMEALine.h"
#include "NMEATenthsInt16.h"
#include "NMEATemperatureUnits.h"

#include "Logger.h"

NMEAMTWMessage::NMEAMTWMessage(const NMEATalker &talker) : NMEAMessage(talker) {
}

bool NMEAMTWMessage::parse(NMEALine &nmeaLine) {
    if (!waterTemperature.extract(nmeaLine, talker, "MTW", "Water Temperature")) {
        return false;
    }

    if (!waterTemperatureUnits.extract(nmeaLine, talker, "MTW")) {
        return false;
    }

    return true;
}

NMEAMsgType::Value NMEAMTWMessage::type() const {
    return NMEAMsgType::MTW;
}

void NMEAMTWMessage::log() const {
    logger() << logDebugNMEA << talker << " MTW: Water temperature " << waterTemperature
             << waterTemperatureUnits << eol;
}

NMEAMTWMessage *parseNMEAMTWMessage(const NMEATalker &talker, NMEALine &nmeaLine,
                                    uint8_t *nmeaMessageBuffer) {
    NMEAMTWMessage *message = new (nmeaMessageBuffer)NMEAMTWMessage(talker);
    if (!message) {
        return nullptr;
    }

    if (!message->parse(nmeaLine)) {
        // Since we use a static buffer and placement new for messages, we don't do a free here.
        return nullptr;
    }

    return message;
}
