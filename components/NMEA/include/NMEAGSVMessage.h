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

#ifndef NMEA_GSV_MESSAGE_H
#define NMEA_GSV_MESSAGE_H

#include "NMEAMessage.h"
#include "NMEAUInt8.h"
#include "NMEAGSVSatelitteInfo.h"
#include "NMEATalker.h"
#include "NMEALine.h"

class NMEAGSVMessage : public NMEAMessage {
    private:
        NMEAUInt8 sentencesInGroup;
        NMEAUInt8 sentenceNumber;
        NMEAUInt8 numberSatellites;
        uint8_t satelittesInMessage;
        NMEAGSVSatelitteInfo satelittes[4];

    public:
        NMEAGSVMessage(NMEATalker &talker);
        bool parse(NMEALine &nmeaLine);
        virtual enum NMEAMsgType type() const override;
        virtual void log() const override;

    friend class NMEADataModelBridge;
};

extern NMEAGSVMessage *parseNMEAGSVMessage(NMEATalker &talker, NMEALine &nmeaLine);

#endif
