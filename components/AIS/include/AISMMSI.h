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

#ifndef AIS_MMSI_H
#define AIS_MMSI_H

#include "etl/bit_stream.h"

#include <stdint.h>

class Logger;

class AISMMSI {
    private:
        uint32_t mmsi;

    public:
        AISMMSI();
        AISMMSI(uint32_t mmsi);
        AISMMSI(etl::bit_stream_reader &streamReader);
        void set(etl::bit_stream_reader &streamReader);
        bool isSet() const;
        bool isAuxiliaryCraft() const;
        bool operator < (const AISMMSI &rhs) const;

        friend Logger & operator << (Logger &logger, const AISMMSI &mmsi);
};

#endif // AIS_MMSI_H
