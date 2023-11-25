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

#include "NMEAVTGMessage.h"
#include "NMEATenthsUInt16.h"
#include "NMEAFAAModeIndicator.h"
#include "NMEATalker.h"
#include "NMEALine.h"
#include "NMEAMessageBuffer.h"

#include "Logger.h"

#include <etl/string_view.h>

NMEAVTGMessage::NMEAVTGMessage(NMEATalker &talker) : NMEAMessage(talker) {
}

bool NMEAVTGMessage::parse(NMEALine &nmeaLine) {
    if (nmeaLine.isEncapsulatedData()) {
        logger << logWarnNMEA << talker << " VTG message in unsupported encapsulated format" << eol;
        return false;
    }

    if (!trackMadeGoodTrue.extract(nmeaLine, talker, "VTG", "Track Made Good", true)) {
        return false;
    }

    // There are two forms of this sentence, one with "T", "M", "N", "K" after each field, and one
    // with out.
    etl::string_view secondWordView;
    if (!nmeaLine.getWord(secondWordView)) {
        logger << logErrorNMEA << talker << " VTG message missing second field" << eol;
        return false;
    }

    bool oldForm = !wordIsT(secondWordView);
    if (!oldForm) {
        if (!nmeaLine.getWord(secondWordView)) {
            logger << logErrorNMEA << talker << " VTG message missing Course Over Ground, Magnetic "
                   << eol;
            return false;
        }
    }

    if (!trackMadeGoodMagnetic.set(secondWordView, true)) {
        logger << logErrorNMEA << talker << " VTG message with a bad Course Over Ground, Magentic"
               << "'" << secondWordView << "'" << eol;
        return false;
    }

    if (!oldForm) {
        if (!extractConstantWord(nmeaLine, "VTG", "M")) {
            return false;
        }
    }

    if (!speedOverGround.extract(nmeaLine, talker, "VTG", "Speed Over Ground")) {
        return false;
    }

   if (!oldForm) {
        if (!extractConstantWord(nmeaLine, "VTG", "N")) {
            return false;
        }
    }

    if (!speedOverGroundKmPerH.extract(nmeaLine, talker, "VTG", "Speed Over Ground km/h")) {
        return false;
    }

   if (!oldForm) {
        if (!extractConstantWord(nmeaLine, "VTG", "K")) {
            return false;
        }
    }

    if (!faaModeIndicator.extract(nmeaLine, talker, "VTG")) {
        return false;
    }

    return true;
}

bool NMEAVTGMessage::wordIsT(const etl::string_view &word) {
    if (word.size() == 1) {
        if (word.front() == 'T') {
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}

enum NMEAMsgType NMEAVTGMessage::type() const {
    return NMEA_MSG_TYPE_VTG;
}

void NMEAVTGMessage::log() const {
    logger << logDebugNMEA << talker << " VTG: " << trackMadeGoodTrue << " "
           << trackMadeGoodMagnetic << " " << speedOverGround << "kn " << speedOverGroundKmPerH
           << "km/h";

    if (faaModeIndicator.hasValue()) {
        logger << " " << faaModeIndicator;
    }
    logger << eol;
}

NMEAVTGMessage *parseNMEAVTGMessage(NMEATalker &talker, NMEALine &nmeaLine) {
    NMEAVTGMessage *message = new (nmeaMessageBuffer)NMEAVTGMessage(talker);
    if (!message) {
        return NULL;
    }

    if (!message->parse(nmeaLine)) {
        // Since we use a static buffer and placement new for messages, we don't do a free here.
        return NULL;
    }

    return message;
}
