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

#include "NMEALongitude.h"
#include "NMEALineWalker.h"
#include "NMEATalker.h"

#include "DataModelStringLeaf.h"

#include "Logger.h"

#include "etl/string_view.h"

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

bool NMEALongitude::extract(NMEALineWalker &lineWalker, NMEATalker &talker, const char *msgType,
                            bool optional) {
    etl::string_view longitudeView;
    if (!lineWalker.getWord(longitudeView) || longitudeView.length() == 0) {
        if (!optional) {
            logger() << logWarnNMEA << talker << " " << msgType << " message missing longitude" << eol;
            return false;
        }

        hasValue = false;
        lineWalker.skipWord();
        return true;
    }

    etl::string_view eastOrWestView;
    if (!lineWalker.getWord(eastOrWestView) || eastOrWestView.length() == 0) {
        if (!optional) {
            logger() << logWarnNMEA << talker << " " << msgType << " message missing E/W" << eol;
            return false;
        }

        hasValue = false;
        return true;
    }

    if (!set(longitudeView, eastOrWestView)) {
        logger() << logWarnNMEA << talker << " " << msgType << " message with bad longitude '"
                 << longitudeView << "' '" << eastOrWestView << "'" << eol;
        hasValue = false;
        return false;
    } else {
        hasValue = true;
    }

    return true;
}

void NMEALongitude::publish(DataModelStringLeaf &leaf) const {
    if (hasValue) {
        switch (eastOrWest) {
            case EAST:
                NMEACoordinate::publish(leaf, "E");
                break;

            case WEST:
                NMEACoordinate::publish(leaf, "W");
        }
    } else {
        leaf.removeValue();
    }
}

void NMEALongitude::log(Logger &logger) const {
    if (hasValue) {
        switch (eastOrWest) {
            case EAST:
                NMEACoordinate::log(logger, "E");
                break;

            case WEST:
                NMEACoordinate::log(logger, "W");
        }
    } else {
        logger << "NA";
    }
}
