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

#include "AISEPFDFixType.h"

#include "Logger.h"

#include "etl/bit_stream.h"

#include <stddef.h>
#include <stdint.h>

AISEPFDFixType::AISEPFDFixType() {
    value = UNDEFINED;
}

AISEPFDFixType::AISEPFDFixType(etl::bit_stream_reader &streamReader) {
    const uint8_t epfdFixTypeCode = etl::read_unchecked<uint8_t>(streamReader, 4);

    value = (enum AISEPFDFixType::Value)epfdFixTypeCode;
}

const char *AISEPFDFixType::name() const {
    switch (value) {
        case GPS:
            return "GPS";
        case GLONASS:
            return "GLONASS";
        case COMBINED_GPS_GLONASS:
            return "Combined GPS/GLONASS";
        case LORAN_C:
            return "Loran-C";
        case CHAYKA:
            return "Chayka";
        case INTEGRATED_NAVIGATION_SYS:
            return "Integrated Navigation System";
        case SURVEYED:
            return "Surveyed";
        case GALILEO:
            return "Galileo";
        case RESERVED_9:
            return "Reserved (9)";
        case RESERVED_10:
            return "Reserved (10)";
        case RESERVED_11:
            return "Reserved (11)";
        case RESERVED_12:
            return "Reserved (12)";
        case RESERVED_13:
            return "Reserved (13)";
        case RESERVED_14:
            return "Reserved (14)";
        case INTERNAL_GNSS:
            return "Internal GNSS";
        case UNDEFINED:
        default:
            return "Undefined";
    }
}

Logger & operator << (Logger &logger, const AISEPFDFixType &epfdFixType) {
    logger << epfdFixType.name();

    return logger;
}
