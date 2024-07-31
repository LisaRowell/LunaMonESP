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

#ifndef NMEA_SPEED_UNITS_H
#define NMEA_SPEED_UNITS_H

#include "LoggableItem.h"

#include "etl/string_view.h"

class NMEALine;
class NMEATalker;
class Logger;

class NMEASpeedUnits : public LoggableItem {
    public:
        enum Value : uint8_t {
            UNKNOWN,
            KNOTS,
            MILES_PER_HOUR,
            KILOMETERS_PER_HOUR
        };

    private:
        Value value;

        bool set(etl::string_view &speedUnitsView);

    public:
        NMEASpeedUnits();
        bool extract(NMEALine &nmeaLine, const NMEATalker &talker, const char *msgType);
        constexpr operator Value() const { return value; }
        virtual void log(Logger &logger) const override;
};

#endif // NMEA_SPEED_UNITS_H
