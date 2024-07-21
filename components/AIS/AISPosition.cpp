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

#include "AISPosition.h"

#include "Logger.h"

#include "etl/bit_stream.h"

#include <stdint.h>

AISPosition::AISPosition() {
    longitudeTenThousandthsMinute = LONGITUDE_UNKNOWN;
    latitudeTenThousandthsMinute = LATITUDE_UNKNOWN;
}

AISPosition::AISPosition(etl::bit_stream_reader &streamReader) {
    longitudeTenThousandthsMinute = etl::read_unchecked<int32_t>(streamReader, 28);
    latitudeTenThousandthsMinute = etl::read_unchecked<int32_t>(streamReader, 27);
}

Logger & operator << (Logger &logger, const AISPosition &position) {
    // We should get more clever with this and use fixed point math.
    if (position.longitudeTenThousandthsMinute == AISPosition::LONGITUDE_UNKNOWN) {
        logger << "?";
    } else {
        const float longitudeDegrees =
            (float)position.longitudeTenThousandthsMinute
                / (AISPosition::MINUTES_PER_DEGREE * 10000);
        logger << longitudeDegrees;
    }

    logger << ",";


    if (position.latitudeTenThousandthsMinute == AISPosition::LATITUDE_UNKNOWN) {
        logger << "?";
    } else {
        const float latitudeDegrees =
            (float)position.latitudeTenThousandthsMinute /
                (AISPosition::MINUTES_PER_DEGREE * 10000);
        logger << latitudeDegrees;
    }

    return logger;
}
