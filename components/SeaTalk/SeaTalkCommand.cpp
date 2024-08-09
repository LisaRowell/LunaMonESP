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
        case DEPTHS_BELOW_TRANSDUCER:
            return "Depths Below Transducer";
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
        default:
            return "Unknown";
    }
}

Logger & operator << (Logger &logger, const SeaTalkCommand &seaTalkCommand) {
    logger << seaTalkCommand.name();

    return logger;
}
