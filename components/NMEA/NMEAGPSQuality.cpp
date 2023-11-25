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

#include "NMEAGPSQuality.h"
#include "NMEALine.h"
#include "NMEATalker.h"

// #include "DataModel/DataModelStringLeaf.h"

#include "CharacterTools.h"
#include "Logger.h"

#include <etl/string_view.h>

#include <ctype.h>

bool NMEAGPSQuality::set(etl::string_view &gpsQualityView) {
    if (gpsQualityView.size() != 1) {
        return false;
    }

    const char gpsQualityChar = gpsQualityView.front();
    if (!isdigit(gpsQualityChar)) {
        return false;
    }
    if (decimalValue(gpsQualityChar) > GPS_QUALITY_SIMULATED_MODE) {
        return false;
    }
    gpsQuality = (GPSQuality)decimalValue(gpsQualityChar);

    return true;
}

bool NMEAGPSQuality::extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType) {
    etl::string_view gpsQualityView;
    if (!nmeaLine.getWord(gpsQualityView)) {
        logger() << logWarnNMEA << talker << " " << msgType << " message missing GPS Quality field"
                 << eol;
        return false;
    }

    if (!set(gpsQualityView)) {
        logger() << logWarnNMEA << talker << " " << msgType
                 << " message with bad GPS Quality field '" << gpsQualityView << "'" << eol;
        return false;
    }

    return true;
}

#if 0
void NMEAGPSQuality::publish(DataModelStringLeaf &leaf) const {
    switch (gpsQuality) {
        case GPS_QUALITY_FIX_NOT_AVAILABLE:
            leaf = "Fix Not Available";
            break;

        case GPS_QUALITY_GPS_FIX:
            leaf = "GPS Fix";
            break;

        case GPS_QUALITY_DIFFERENTIAL_GPS_FIX:
            leaf = "Differential GPS Fix";
            break;

        case GPS_QUALITY_PPS_FIX:
            leaf = "PPS Fix";
            break;

        case GPS_QUALITY_REAL_TIME_KINEMATIC:
            leaf = "Real Time Kinematic";
            break;

        case GPS_QUALITY_FLOAT_RTK:
            leaf = "Float RTK";
            break;

        case GPS_QUALITY_ESTIMATED:
            leaf = "Estimated";
            break;

        case GPS_QUALITY_MANUAL_INPUT_MODE:
            leaf = "Manual Input Mode";
            break;

        case GPS_QUALITY_SIMULATED_MODE:
            leaf = "Simulated Mode";
    }
}
#endif

void NMEAGPSQuality::log(Logger &logger) const {
    switch (gpsQuality) {
        case GPS_QUALITY_FIX_NOT_AVAILABLE:
            logger << "Fix Not Available";
            break;

        case GPS_QUALITY_GPS_FIX:
            logger << "GPS Fix";
            break;

        case GPS_QUALITY_DIFFERENTIAL_GPS_FIX:
            logger << "Differential GPS Fix";
            break;

        case GPS_QUALITY_PPS_FIX:
            logger << "PPS Fix";
            break;

        case GPS_QUALITY_REAL_TIME_KINEMATIC:
            logger << "Real Time Kinematic";
            break;

        case GPS_QUALITY_FLOAT_RTK:
            logger << "Float RTK";
            break;

        case GPS_QUALITY_ESTIMATED:
            logger << "Estimated";
            break;

        case GPS_QUALITY_MANUAL_INPUT_MODE:
            logger << "Manual Input Mode";
            break;

        case GPS_QUALITY_SIMULATED_MODE:
            logger << "Simulated Mode";
    }
}
