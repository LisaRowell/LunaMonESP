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

#include "NMEATemperatureUnits.h"
#include "NMEALine.h"
#include "NMEATalker.h"

#include "Logger.h"

#include "etl/string_view.h"

NMEATemperatureUnits::NMEATemperatureUnits() : value(UNKNOWN) {
}

bool NMEATemperatureUnits::set(etl::string_view &temperatureUnitsView) {
    if (temperatureUnitsView.size() != 1) {
        return false;
    }

    switch (temperatureUnitsView.front()) {
        case 'C':
            value = CELSIUS;
            return true;

        case 'F':
            value = FAHRENHEIT;
            return true;

        default:
            return false;
    }
}

bool NMEATemperatureUnits::extract(NMEALine &nmeaLine, const NMEATalker &talker,
                                   const char *msgType) {
    etl::string_view temperatureUnitsView;
    if (!nmeaLine.getWord(temperatureUnitsView)) {
        logger() << logWarnNMEA << talker << " " << msgType
                 << " message missing Temperature Units field" << eol;
        return false;
    }
    if (!set(temperatureUnitsView)) {
        logger() << logWarnNMEA << talker << " " << msgType
                 << " message with bad Temperature Units field '" << temperatureUnitsView << "'"
                 << eol;
        return false;
    }

    return true;
}

void NMEATemperatureUnits::log(Logger &logger) const {
    switch (value) {
        case CELSIUS:
            logger << "C";
            break;

        case FAHRENHEIT:
            logger << "F";
            break;
        
        default:
            logger << "?";
    }
}
