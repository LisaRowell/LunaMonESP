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

#include "NMEASpeedUnits.h"
#include "NMEALineWalker.h"
#include "NMEATalker.h"

#include "Logger.h"

#include "etl/string_view.h"

NMEASpeedUnits::NMEASpeedUnits() : value(UNKNOWN) {
}

bool NMEASpeedUnits::set(etl::string_view &speedUnitsView) {
    if (speedUnitsView.size() != 1) {
        return false;
    }

    switch (speedUnitsView.front()) {
        case 'N':
            value = KNOTS;
            return true;

        case 'M':
            value = MILES_PER_HOUR;
            return true;

        case 'K':
            value = KILOMETERS_PER_HOUR;
            return true;

        default:
            return false;
    }
}

bool NMEASpeedUnits::extract(NMEALineWalker &lineWalker, const NMEATalker &talker,
                             const char *msgType) {
    etl::string_view speedUnitsView;
    if (!lineWalker.getWord(speedUnitsView)) {
        logger() << logWarnNMEA << talker << " " << msgType << " message missing Speed Units field"
                 << eol;
        return false;
    }
    if (!set(speedUnitsView)) {
        logger() << logWarnNMEA << talker << " " << msgType
                 << " message with bad Speed Units field '" << speedUnitsView << "'" << eol;
        return false;
    }

    return true;
}

void NMEASpeedUnits::log(Logger &logger) const {
    switch (value) {
        case KNOTS:
            logger << "kn";
            break;

        case MILES_PER_HOUR:
            logger << "mph";
            break;

        case KILOMETERS_PER_HOUR:
            logger << "km/h";
            break;
        
        default:
            logger << "?";
    }
}
