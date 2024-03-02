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

#ifndef NMEA_GPS_FIX_MODE_H
#define NMEA_GPS_FIX_MODE_H

#include "LoggableItem.h"

#include "etl/string_view.h"

class NMEALine;
class NMEATalker;
class DataModelStringLeaf;
class DataModelStringLeaf;
class Logger;

class NMEAGPSFixMode : public LoggableItem {
    private:
        enum GPSFixMode {
            GPS_FIX_MODE_NO_FIX = 1,
            GPS_FIX_MODE_2D = 2,
            GPS_FIX_MODE_3D = 3
        };
        GPSFixMode gpsFixMode;

        bool set(etl::string_view &gpsQualityStr);

    public:
        bool extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType);
        void publish(DataModelStringLeaf &leaf) const;
        virtual void log(Logger &logger) const override;
};

#endif
