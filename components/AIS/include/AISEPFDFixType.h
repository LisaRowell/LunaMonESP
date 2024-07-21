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

#ifndef AIS_EPFD_FIX_TYPE_H
#define AIS_EPFD_FIX_TYPE_H

#include "etl/bit_stream.h"

#include <stdint.h>

class Logger;

class AISEPFDFixType {
    public:
        enum Value : uint8_t {
            UNDEFINED,
            GPS,
            GLONASS,
            COMBINED_GPS_GLONASS,
            LORAN_C,
            CHAYKA,
            INTEGRATED_NAVIGATION_SYS,
            SURVEYED,
            GALILEO,
            RESERVED_9,
            RESERVED_10,
            RESERVED_11,
            RESERVED_12,
            RESERVED_13,
            RESERVED_14,
            INTERNAL_GNSS
        };

    private:
        Value value;

    public:
        AISEPFDFixType();
        AISEPFDFixType(etl::bit_stream_reader &streamReader);
        constexpr operator Value() const { return value; }
        const char *name() const;
        explicit operator bool() const = delete;

        friend Logger & operator << (Logger &logger, const AISEPFDFixType &shipType);
};

#endif // AIS_EPFD_FIX_TYPE_H
