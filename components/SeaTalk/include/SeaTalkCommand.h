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

#ifndef SEATALK_COMMAND_H
#define SEATALK_COMMAND_H

#include <stdint.h>

class Logger;

class SeaTalkCommand {
    public:
        enum Value : uint8_t {
            DEPTHS_BELOW_TRANSDUCER         = 0x00,
            APPARENT_WIND_ANGLE             = 0x10,
            APPARENT_WIND_SPEED             = 0x11,
            SPEED_THROUGH_WATER_V1          = 0x20,
            WATER_TEMPERATURE_V1            = 0x23,
            DISPLAY_UNITS_MILEAGE_AND_SPEED = 0x24,
            SPEED_THROUGH_WATER_V2          = 0x26,
            WATER_TEMPERATURE_V2            = 0x27,
            SET_LAMP_INTENSITY              = 0x30
        };

    private:
        Value value;

    public:
        SeaTalkCommand(uint8_t value);
        constexpr operator Value() const { return value; }
        const char *name() const;
        explicit operator bool() const = delete;

        friend Logger & operator << (Logger &logger, const SeaTalkCommand &seaTalkCommand);
};

#endif // SEATALK_COMMAND_H
