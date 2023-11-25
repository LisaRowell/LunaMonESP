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

#include "NMEALongitude.h"
#include "NMEALine.h"
#include "NMEATalker.h"

// #include "DataModel/DataModelStringLeaf.h"

#include "Logger.h"

#include <etl/string_view.h>

bool NMEALongitude::set(const etl::string_view &longitudeView,
                        const etl::string_view &eastOrWestView) {
    if (longitudeView.size() < 5) {
        return false;
    }

    etl::string_view degreesView(longitudeView.data(), 3);
    if (!setDegrees(degreesView, 180)) {
        return false;
    }

    etl::string_view minutesView(longitudeView.begin() + 3, longitudeView.end());
    if (!setMinutes(minutesView)) {
        return false;
    }

    if (eastOrWestView.size() != 1) {
        return false;
    }
    switch (eastOrWestView.front()) {
        case 'E':
            eastOrWest = EAST;
            return true;
        case 'W':
            eastOrWest = WEST;
            return true;
        default:
            return false;
    }
}

bool NMEALongitude::extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType) {
    etl::string_view longitudeView;
    if (!nmeaLine.getWord(longitudeView)) {
        logger << logWarnNMEA << talker << " " << msgType << " message missing longitude" << eol;
        return false;
    }

    etl::string_view eastOrWestView;
    if (!nmeaLine.getWord(eastOrWestView)) {
        logger << logWarnNMEA << talker << " " << msgType << " message missing E/W" << eol;
        return false;
    }

    if (!set(longitudeView, eastOrWestView)) {
        logger << logWarnNMEA << talker << " " << msgType << " message with bad longitude '"
               << longitudeView << "' '" << eastOrWestView << "'" << eol;
        return false;
    }

    return true;
}

#if 0
void NMEALongitude::publish(DataModelStringLeaf &leaf) const {
    switch (eastOrWest) {
        case EAST:
            NMEACoordinate::publish(leaf, "E");
            break;

        case WEST:
            NMEACoordinate::publish(leaf, "W");
    }
}
#endif

void NMEALongitude::log(Logger &logger) const {
    NMEACoordinate::log(logger);

    switch (eastOrWest) {
        case EAST:
            logger << "E";
            break;

        case WEST:
            logger << "W";
    }
}
