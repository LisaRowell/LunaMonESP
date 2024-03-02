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

#ifndef NMEA_GGA_MESSAGE_H
#define NMEA_GGA_MESSAGE_H

#include "NMEAMessage.h"
#include "NMEATime.h"
#include "NMEADataValid.h"
#include "NMEALatitude.h"
#include "NMEALongitude.h"
#include "NMEAGPSQuality.h"
#include "NMEAHundredthsUInt16.h"
#include "NMEATenthsInt16.h"
#include "NMEATenthsUInt16.h"
#include "NMEAUInt16.h"
#include "NMEATalker.h"
#include "NMEALine.h"

class NMEAGGAMessage : public NMEAMessage {
    private:
        NMEATime time;
        NMEALatitude latitude;
        NMEALongitude longitude;
        NMEAGPSQuality gpsQuality;
        NMEAUInt16 numberSatellites;
        NMEAHundredthsUInt16 horizontalDilutionOfPrecision;
        NMEATenthsInt16 antennaAltitude;
        NMEATenthsInt16 geoidalSeparation;
        NMEATenthsUInt16 gpsDataAge;
        NMEAUInt16 differentialReferenceStation;

    public:
        NMEAGGAMessage(NMEATalker &talker);
        bool parse(NMEALine &nmeaLine);
        virtual enum NMEAMsgType type() const override;
        virtual void log() const override;

    friend class NMEAGPSBridge;
};

extern NMEAGGAMessage *parseNMEAGGAMessage(NMEATalker &talker, NMEALine &nmeaLine);

#endif
