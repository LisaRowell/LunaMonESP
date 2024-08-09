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

#ifndef SEATALK_LAMP_INTENSITY_H
#define SEATALK_LAMP_INTENSITY_H

#include <stdint.h>

class Logger;

class SeaTalkLampIntensity {
    public:
        enum Value : uint8_t {
            L0    = 0x00,
            L1    = 0x04,
            L2    = 0x08,
            L3    = 0x0c
        };

    private:
        Value value;

        bool intensityValid() const;

    public:
        SeaTalkLampIntensity(uint8_t value);
        constexpr operator Value() const { return value; }
        const char *name() const;
        explicit operator bool() const = delete;

        friend Logger & operator << (Logger &logger, const SeaTalkLampIntensity &lampIntensity);
};

#endif // SEATALK_LAMP_INTENSITY_H
