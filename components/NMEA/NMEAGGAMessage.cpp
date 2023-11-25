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

#include "NMEAGGAMessage.h"
#include "NMEATime.h"
#include "NMEADataValid.h"
#include "NMEALatitude.h"
#include "NMEALongitude.h"
#include "NMEAHundredthsUInt16.h"
#include "NMEATenthsInt16.h"
#include "NMEATalker.h"
#include "NMEALine.h"
#include "NMEAMessageBuffer.h"

#include "StringTools.h"
#include "Logger.h"

NMEAGGAMessage::NMEAGGAMessage(NMEATalker &talker) : NMEAMessage(talker) {
}

bool NMEAGGAMessage::parse(NMEALine &nmeaLine) {
    if (nmeaLine.isEncapsulatedData()) {
        logger << logWarnNMEA << talker << " GGA message in unsupported encapsulated format" << eol;
        return false;
    }

    if (!time.extract(nmeaLine, talker, "GGA")) {
        return false;
    }

    if (!latitude.extract(nmeaLine, talker, "GGA")) {
        return false;
    }

    if (!longitude.extract(nmeaLine, talker, "GGA")) {
        return false;
    }

    if (!gpsQuality.extract(nmeaLine, talker, "GGA")) {
        return false;
    }

    if (!numberSatellites.extract(nmeaLine, talker, "GGA", "Number Satellites", false, 12)) {
        return false;
    }

    if (!horizontalDilutionOfPrecision.extract(nmeaLine, talker, "GGA",
                                               "Horizontal Dilution of Precision")) {
        return false;
    }

    if (!antennaAltitude.extract(nmeaLine, talker, "GGA", "Antenna Altitude")) {
        return false;
    }

    if (!extractConstantWord(nmeaLine, "GGA", "M")) {
        return false;
    }

    if (!geoidalSeparation.extract(nmeaLine, talker, "GGA", "Geoidal Separation")) {
        return false;
    }

    if (!extractConstantWord(nmeaLine, "GGA", "M")) {
        return false;
    }

    if (!gpsDataAge.extract(nmeaLine, talker, "GGA", "GPS Data Age", true)) {
        return false;
    }

    if (!differentialReferenceStation.extract(nmeaLine, talker, "GGA",
                                              "Differential Reference Station", true, 1023)) {
        return false;
    }

    return true;
}

enum NMEAMsgType NMEAGGAMessage::type() const {
    return NMEA_MSG_TYPE_GGA;
}

void NMEAGGAMessage::log() const {
    logger << logDebugNMEA << talker << " GGA: " << time << " " << latitude << " " << longitude
           << " " << gpsQuality << " " << numberSatellites << " " << horizontalDilutionOfPrecision
           << " " << antennaAltitude << "m " << geoidalSeparation << "m";

    if (gpsDataAge.hasValue()) {
        logger << " " << gpsDataAge;
    }

    if (differentialReferenceStation.hasValue()) {
        logger << " " << differentialReferenceStation;
    }

    logger << eol;
}

NMEAGGAMessage *parseNMEAGGAMessage(NMEATalker &talker, NMEALine &nmeaLine) {
    NMEAGGAMessage *message = new (nmeaMessageBuffer)NMEAGGAMessage(talker);
    if (!message) {
        return NULL;
    }

    if (!message->parse(nmeaLine)) {
        // Since we use a static buffer and placement new for messages, we don't do a free here.
        return NULL;
    }

    return message;
}
