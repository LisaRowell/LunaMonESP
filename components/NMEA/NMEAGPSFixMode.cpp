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

#include "NMEAGPSFixMode.h"
#include "NMEALine.h"
#include "NMEATalker.h"

// #include "DataModel/DataModelStringLeaf.h"

#include "CharacterTools.h"
#include "Logger.h"

#include "etl/string_view.h"

#include <ctype.h>

bool NMEAGPSFixMode::set(etl::string_view &gpsFixModeView) {
    if (gpsFixModeView.size() != 1) {
        return false;
    }

    const char gpsFixModeChar = gpsFixModeView.front();
    if (!isdigit(gpsFixModeChar)) {
        return false;
    }
    if (decimalValue(gpsFixModeChar) > GPS_FIX_MODE_3D) {
        return false;
    }
    gpsFixMode = (GPSFixMode)decimalValue(gpsFixModeChar);

    return true;
}

bool NMEAGPSFixMode::extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType) {
    etl::string_view gpsFixModeView;
    if (!nmeaLine.getWord(gpsFixModeView)) {
        logger() << logWarnNMEA << talker << " " << msgType << " message missing GPS Fix Mode field"
                 << eol;
        return false;
    }

    if (!set(gpsFixModeView)) {
        logger() << logWarnNMEA << talker << " " << msgType
                 << " message with bad GPS Fix Mode field '" << gpsFixModeView << "'" << eol;
        return false;
    }

    return true;
}

#if 0
void NMEAGPSFixMode::publish(DataModelStringLeaf &leaf) const {
    switch (gpsFixMode) {
        case GPS_FIX_MODE_NO_FIX:
            leaf = "None";
            break;

        case GPS_FIX_MODE_2D:
            leaf = "2D";
            break;

        case GPS_FIX_MODE_3D:
            leaf = "3D";
    }
}
#endif

void NMEAGPSFixMode::log(Logger &logger) const {
    switch (gpsFixMode) {
        case GPS_FIX_MODE_NO_FIX:
            logger << "No Fix";
            break;

        case GPS_FIX_MODE_2D:
            logger << "2D Fix";
            break;

        case GPS_FIX_MODE_3D:
            logger << "3D Fix";
    }
}
