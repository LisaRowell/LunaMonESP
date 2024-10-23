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

#include "SeaTalkCommand.h"

#include "Logger.h"

#include <stdint.h>

SeaTalkCommand::SeaTalkCommand(uint8_t value) {
    this->value = (enum Value)value;
}

const char *SeaTalkCommand::name() const {
    switch (value) {
        case DEPTH_BELOW_TRANSDUCER:
            return "Depth Below Transducer";
        case APPARENT_WIND_ANGLE:
            return "Apparent Wind Angle";
        case APPARENT_WIND_SPEED:
            return "Apparent Wind Speed";
        case SPEED_THROUGH_WATER_V1:
            return "Speed Through Water V1";
        case WATER_TEMPERATURE_V1:
            return "Water Temperature V1";
        case DISPLAY_UNITS_MILEAGE_AND_SPEED:
            return "Display Units For Mileage & Speed";
        case SPEED_THROUGH_WATER_V2:
            return "Speed Through Water V2";
        case WATER_TEMPERATURE_V2:
            return "Water Temperature V2";
        case SET_LAMP_INTENSITY:
            return "Set lamp Intensity";
        case LATITUDE_POSITION:
            return "Latitude Position";
        case LONGITUDE_POSITION:
            return "Longitude Position";
        case SPEED_OVER_GROUND:
            return "Speed Over Ground";
        case COURSE_OVER_GROUND:
            return "Course Over Ground";
        case HOURS_MINUTES_SECONDS:
            return "Time";
        case YEAR_MONTH_DAY:
            return "Date";
        case SATELLITE_INFO:
            return "Satellite Info";
        case RAW_LATITUDE_AND_LONGITUDE:
            return "Raw Latitude and Longitude";
        case COUNT_DOWN_TIMER:
            return "Count Down Timer";
        case E80_START_UP:
            return "E80_START_UP";
        case AUTO_PILOT_STATUS:
            return "Auto pilot status";
        case AUTO_PILOT_HEADING_COURSE_RUDDER:
            return "Auto pilot heading, course and rudder";
        case DEVICE_IDENTIFICATION:
            return "Device identification";
        case MAGNETIC_VARIATION:
            return "Magnetic Variation";
        case AUTO_PILOT_HEADING_AND_RUDDER:
            return "Auto pilot heading and rudder";
        case GPS_AND_DGPS_INFO:
            return "GPS and DGPS Info";
        case UNKNOWN_GPS_COMMAND_A7:
            return "Unknown GPS Command A7";
        default:
            return "Unknown";
    }
}

Logger & operator << (Logger &logger, const SeaTalkCommand &seaTalkCommand) {
    logger << seaTalkCommand.name();

    return logger;
}
