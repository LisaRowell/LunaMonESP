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

#include "NMEAGSAMessage.h"
#include "NMEAGPSFixMode.h"
#include "NMEATalker.h"
#include "NMEAMsgType.h"
#include "NMEALineWalker.h"

#include "StringTools.h"
#include "Logger.h"

#include "etl/string_view.h"

NMEAGSAMessage::NMEAGSAMessage(const NMEATalker &talker) : NMEAMessage(talker) {
}

bool NMEAGSAMessage::parse(NMEALineWalker &lineWalker) {
    etl::string_view manualOrAutomaticModeView;
    if (!lineWalker.getWord(manualOrAutomaticModeView)) {
        logger() << logWarnNMEA << talker
                 << " GSA message missing Manual or Automatic Mode indicator" << eol;
        return false;
    }
    if (manualOrAutomaticModeView.size() == 1) {
        switch (manualOrAutomaticModeView.front()) {
            case 'A':
                automaticMode = true;
                break;
            case 'M':
                automaticMode = false;
                break;
            default:
                logger() << logWarnNMEA << talker
                         << " GSA message with bad Manual or Automatic Mode indicator" << eol;
                return false;
        }
    } else {
        logger() << logWarnNMEA << talker
                 << " GSA message with bad Manual or Automatic Mode indicator" << eol;
        return false;
    }

    if (!gpsFixMode.extract(lineWalker, talker, "GSA")) {
        return false;
    }

    unsigned satelliteIndex;
    for (satelliteIndex = 0; satelliteIndex < 12; satelliteIndex++) {
        if (!satelliteIDs[satelliteIndex].extract(lineWalker, talker, "GSA", "Satellite ID", true)) {
            return false;
        }
    }

    if (!pdop.extract(lineWalker, talker, "GSA", "PDOP")) {
        return false;
    }
    if (!hdop.extract(lineWalker, talker, "GSA", "HDOP")) {
        return false;
    }
    if (!vdop.extract(lineWalker, talker, "GSA", "VDOP")) {
        return false;
    }

    return true;
}

NMEAMsgType::Value NMEAGSAMessage::type() const {
    return NMEAMsgType::GSA;
}

void NMEAGSAMessage::log() const {
    logger() << logDebugNMEA << talker << " GSA: ";

    if (automaticMode) {
        logger() << "Automatic ";
    } else {
        logger() << "Manual ";
    }

    logger() << gpsFixMode << " Satellites ";

    unsigned satelliteIndex;
    for (satelliteIndex = 0; satelliteIndex < 12; satelliteIndex++) {
        logger() << satelliteIDs[satelliteIndex] << " ";
    }
 
    logger() << "PDOP " << pdop << " HDOP " << hdop << " VDOP " << vdop << eol;
}

NMEAGSAMessage *parseNMEAGSAMessage(const NMEATalker &talker, NMEALineWalker &lineWalker,
                                    uint8_t *nmeaMessageBuffer) {
    NMEAGSAMessage *message = new (nmeaMessageBuffer)NMEAGSAMessage(talker);
    if (!message) {
        return nullptr;
    }

    if (!message->parse(lineWalker)) {
        // Since we use a static buffer and placement new for messages, we don't do a free here.
        return nullptr;
    }

    return message;
}
