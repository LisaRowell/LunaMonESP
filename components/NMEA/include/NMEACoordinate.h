/*
 * This file is part of LunaMon (https://github.com/LisaRowell/LunaMonESP)
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

#ifndef NMEA_COORDINATE_H
#define NMEA_COORDINATE_H

// #include "DataModel/DataModelStringLeaf.h"

#include "etl/string_view.h"

#include <stdint.h>

class Logger;

class NMEACoordinate {
    protected:
        uint8_t degrees;
        float minutes;

        bool setDegrees(const etl::string_view &degreesView, uint8_t maxDegrees);
        bool setMinutes(const etl::string_view &minutesView);
        void log(Logger &logger) const;
        void snprint(char *string, size_t maxLength) const;
//        void publish(DataModelStringLeaf &leaf, const char *suffix) const;
};

#endif
