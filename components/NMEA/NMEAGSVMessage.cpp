/*
 * This file is part of LunaMon (https://github.com/LisaRowell/LunaMon)
 * Copyright (C) 2021-2023 Lisa Rowell
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

#include "NMEAGSVMessage.h"
#include "NMEAMsgType.h"
#include "NMEATalker.h"
#include "NMEALine.h"
#include "NMEAUInt8.h"
#include "NMEAMessageBuffer.h"

#include "StringTools.h"
#include "Logger.h"

NMEAGSVMessage::NMEAGSVMessage(NMEATalker &talker) : NMEAMessage(talker) {
}

bool NMEAGSVMessage::parse(NMEALine &nmeaLine) {
    if (nmeaLine.isEncapsulatedData()) {
        logger() << logWarnNMEA << talker << " GSV message in unsupported encapsulated format"
                 << eol;
        return false;
    }

    if (!sentencesInGroup.extract(nmeaLine, talker, "GSV", "Sentences in Group", false, 9)) {
        return false;
    }

    if (!sentenceNumber.extract(nmeaLine, talker, "GSV", "Sentence Number", false, 9)) {
        return false;
    }

    if (!numberSatellites.extract(nmeaLine, talker, "GSV", "Number Satellites")) {
        return false;
    }

    bool endOfInput;
    for (satelittesInMessage = 0, endOfInput = false; !endOfInput && satelittesInMessage < 4;) {
        if (!satelittes[satelittesInMessage].extract(nmeaLine, talker, endOfInput)) {
            return false;
        }
        if (!endOfInput) {
            satelittesInMessage++;
        }
    }

    return true;
}

enum NMEAMsgType NMEAGSVMessage::type() const {
    return NMEA_MSG_TYPE_GSV;
}

void NMEAGSVMessage::log() const {
    logger() << logDebugNMEA << talker << " GSV: " << sentenceNumber << " of " << sentencesInGroup
             << " Satelllites " << numberSatellites;

    unsigned satelitteIndex;
    for (satelitteIndex = 0; satelitteIndex < satelittesInMessage; satelitteIndex++) {
        logger() << " " << satelittes[satelitteIndex];
    }

    logger() << eol;
}

NMEAGSVMessage *parseNMEAGSVMessage(NMEATalker &talker, NMEALine &nmeaLine) {
    NMEAGSVMessage *message = new (nmeaMessageBuffer)NMEAGSVMessage(talker);
    if (!message) {
        return NULL;
    }

    if (!message->parse(nmeaLine)) {
        // Since we use a static buffer and placement new for messages, we don't do a free here.
        return NULL;
    }

    return message;
}
