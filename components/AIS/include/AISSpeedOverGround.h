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

#ifndef AIS_SPEED_OVER_GROUND_H
#define AIS_SPEED_OVER_GROUND_H

#include "etl/bit_stream.h"

#include <stdint.h>

class Logger;

class AISSpeedOverGround {
    private:
        static constexpr uint16_t SPEED_OVER_GROUND_OVER_102_KN = 1022;
        static constexpr uint16_t SPEED_OVER_GROUND_NOT_AVAILABLE = 1023;

        uint16_t speedCode;

    public:
        AISSpeedOverGround();
        AISSpeedOverGround(uint8_t speedKn);
        bool isValid() const;
        bool isZero() const;
        AISSpeedOverGround(etl::bit_stream_reader &streamReader);

        friend Logger & operator << (Logger &logger, const AISSpeedOverGround &speedOverGround);
};

#endif // AIS_SPEED_OVER_GROUND_H
