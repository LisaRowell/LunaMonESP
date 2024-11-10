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

#include "NMEALatitude.h"
#include "NMEALineWalker.h"
#include "NMEATalker.h"

#include "DataModelStringLeaf.h"

#include "Logger.h"

#include "etl/string_view.h"

bool NMEALatitude::set(const etl::string_view &latitudeView,
                       const etl::string_view &northOrSouthView) {
    if (latitudeView.size() < 4) {
        return false;
    }

    etl::string_view degreesView(latitudeView.data(), 2);
    if (!setDegrees(degreesView, 90)) {
        return false;
    }

    etl::string_view minutesView(latitudeView.begin() + 2, latitudeView.end());
    if (!setMinutes(minutesView)) {
        return false;
    }

    if (northOrSouthView.size() != 1) {
        return false;
    }
    switch (northOrSouthView.front()) {
        case 'N':
            northOrSouth = NORTH;
            return true;
        case 'S':
            northOrSouth = SOUTH;
            return true;
        default:
            return false;
    }

    return true;
}

bool NMEALatitude::extract(NMEALineWalker &lineWalker, NMEATalker &talker, const char *msgType,
                           bool optional) {
    etl::string_view latitudeView;
    if (!lineWalker.getWord(latitudeView) || latitudeView.length() == 0) {
        if (!optional) {
            logger() << logWarnNMEA << talker << " " << msgType << " message missing latitude"
                     << eol;
            return false;
        }

        hasValue = false;
        lineWalker.skipWord();
        return true;
    }

    etl::string_view northOrSouthView;
    if (!lineWalker.getWord(northOrSouthView) || northOrSouthView.length() == 0) {
        if (!optional) {
            logger() << logWarnNMEA << talker << " " << msgType << " message missing N/S" << eol;
            return false;
        }

        hasValue = false;
        return true;
    }

    if (!set(latitudeView, northOrSouthView)) {
        logger() << logWarnNMEA << talker << " " << msgType << " message with bad latitude '"
                 << latitudeView << "' '" << northOrSouthView << "'" << eol;
        hasValue = false;
        return false;
    } else {
        hasValue = true;
    }

    return true;
}

void NMEALatitude::publish(DataModelStringLeaf &leaf) const {
    if (hasValue) {
        switch (northOrSouth) {
            case NORTH:
                NMEACoordinate::publish(leaf, "N");
                break;

            case SOUTH:
                NMEACoordinate::publish(leaf, "S");
        }
    } else {
        leaf.removeValue();
    }
}

void NMEALatitude::log(Logger &logger) const {
    if (hasValue) {
        switch (northOrSouth) {
            case NORTH:
                NMEACoordinate::log(logger, "N");
                break;

            case SOUTH:
                NMEACoordinate::log(logger, "S");
        }
    } else {
        logger << "NA";
    }
}
