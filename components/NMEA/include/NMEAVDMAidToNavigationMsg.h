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

#ifndef NMEA_VDM_AID_TO_NAVIGATION_MSG_H
#define NMEA_VDM_AID_TO_NAVIGATION_MSG_H

#include "NMEAMessage.h"
#include "NMEAMMSI.h"
#include "NMEANavAidType.h"
#include "NMEAAISName.h"
#include "NMEAEPFDFixType.h"

#include "etl/bit_stream.h"

#include <stdint.h>
#include <stddef.h>

class NMEATalker;

class NMEAVDMAidToNavigationMsg : public NMEAMessage {
    private:
        uint8_t repeatIndicator;
        uint32_t mmsi;
        NMEANavAidType navAidType;
        NMEAAISName name;
        bool positionAccuracy;
        int32_t longitudeTenThousandsNM;
        int32_t latitudeTenThousandsNM;
        uint16_t dimensionToBowM;
        uint16_t dimensionToSternM;
        uint8_t dimensionToPortM;
        uint8_t dimensionToStarboardM;
        NMEAEPFDFixType epfdFixType;
        uint8_t utcSecond;
        bool offPosition;
        uint8_t regionalReserved;
        bool raim;
        bool virtualAid;
        bool assignedMode;
        uint8_t spare;

    public:
        NMEAVDMAidToNavigationMsg(NMEATalker &talker);
        bool parse(etl::bit_stream_reader &streamReader, size_t messageSizeInBits);
        virtual enum NMEAMsgType type() const override;
        virtual void log() const override;
};

#endif //NMEA_AID_TO_NAVIGATION_MSG_H
