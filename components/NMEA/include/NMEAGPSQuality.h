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

#ifndef NMEA_GPS_QUALITY_H
#define NMEA_GPS_QUALITY_H

#include "LoggableItem.h"

#include "etl/string_view.h"

class DataModelStringLeaf;
class NMEALineWalker;
class NMEATalker;
class Logger;

class NMEAGPSQuality : public LoggableItem {
    private:
        enum GPSQuality {
            GPS_QUALITY_FIX_NOT_AVAILABLE = 0,
            GPS_QUALITY_GPS_FIX = 1,
            GPS_QUALITY_DIFFERENTIAL_GPS_FIX = 2,
            GPS_QUALITY_PPS_FIX = 3,
            GPS_QUALITY_REAL_TIME_KINEMATIC = 4,
            GPS_QUALITY_FLOAT_RTK = 5,
            GPS_QUALITY_ESTIMATED = 6,
            GPS_QUALITY_MANUAL_INPUT_MODE = 7,
            GPS_QUALITY_SIMULATED_MODE = 8
        };
        GPSQuality gpsQuality;

        bool set(etl::string_view &gpsQualityView);

    public:
        bool extract(NMEALineWalker &lineWalker, NMEATalker &talker, const char *msgType);
        void publish(DataModelStringLeaf &leaf) const;
        virtual void log(Logger &logger) const override;
};

#endif
