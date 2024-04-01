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

#include "NMEAEPFDFixType.h"

#include "Logger.h"

#include "etl/bit_stream.h"

#include <stdint.h>

void NMEAEPFDFixType::parse(etl::bit_stream_reader &streamReader) {
        type = (enum EPFDFixTypeEnum)etl::read_unchecked<uint8_t>(streamReader, 4);
}

const char *NMEAEPFDFixType::toString() const {
    switch (type) {
        case EPFD_FIX_TYPE_UNDEFINED:
            return "Undefined";
        case EPFD_FIX_TYPE_GPS:
            return "GPS";
        case EPFD_FIX_TYPE_GLONASS:
            return "GLONASS";
        case EPFD_FIX_TYPE_COMBINED_GPS_GLONASS:
            return "Combined GPS/GLONASS";
        case EPFD_FIX_TYPE_LORAN_C:
            return "Loran-C";
        case EPFD_FIX_TYPE_CHAYKA:
            return "Chayka";
        case EPFD_FIX_TYPE_INTEGRATED_NAV_SYS:
            return "Integrated navigation system";
        case EPFD_FIX_TYPE_SURVEYED:
            return "Surveyed";
        case EPFD_FIX_TYPE_GALILEO:
            return "Galileo";
        case EPFD_FIX_TYPE_RESERVED_9:
            return "Reserved(9)";
        case EPFD_FIX_TYPE_RESERVED_10:
            return "Reserved(10)";
        case EPFD_FIX_TYPE_RESERVED_11:
            return "Reserved(11)";
        case EPFD_FIX_TYPE_RESERVED_12:
            return "Reserved(12)";
        case EPFD_FIX_TYPE_RESERVED_13:
            return "Reserved(13)";
        case EPFD_FIX_TYPE_RESERVED_14:
            return "Reserved(14)";
        case EPFD_FIX_TYPE_INTERNAL_GNSS:
            return "Internal GNSS";
        default:
            return "Unknown";
    }
}

void NMEAEPFDFixType::log(Logger &logger) const {
    logger << toString();
}
