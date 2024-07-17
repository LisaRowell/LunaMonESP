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

#ifndef AIS_SHIP_DIMENSIONS_H
#define AIS_SHIP_DIMENSIONS_H

#include "etl/bit_stream.h"

#include <stdint.h>

class Logger;

class AISShipDimensions {
    private:
        uint16_t _lengthM;
        uint8_t _widthM;
        static constexpr uint16_t LARGE_VESSEL_LENGTH = 511;
        static constexpr uint8_t LARGE_VESSEL_WIDTH = 63;

    public:
        AISShipDimensions();
        AISShipDimensions(etl::bit_stream_reader &streamReader);
        void set(etl::bit_stream_reader &streamReader);
        bool isSet() const;
        uint16_t lengthM() const;
        uint8_t widthM() const;

        friend Logger & operator << (Logger &logger, const AISShipDimensions &dimensions);
};

#endif // AIS_SHIP_DIMENSIONS_H
