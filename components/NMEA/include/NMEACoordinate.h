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

#ifndef NMEA_COORDINATE_H
#define NMEA_COORDINATE_H

#include "etl/string_view.h"

#include <stdint.h>

class DataModelStringLeaf;
class Logger;

class NMEACoordinate {
    private:
        static constexpr size_t coordinateLength = 20;

    protected:
        uint8_t degrees;
        float minutes;
        bool hasValue;

        NMEACoordinate();
        bool setDegrees(const etl::string_view &degreesView, uint8_t maxDegrees);
        bool setMinutes(const etl::string_view &minutesView);
        void log(Logger &logger, const char *suffix) const;
        void snprint(char *string, size_t maxLength) const;
        void publish(DataModelStringLeaf &leaf, const char *suffix) const;
};

#endif
