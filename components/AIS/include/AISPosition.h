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

#ifndef AIS_POSITION_H
#define AIS_POSITION_H

#include "etl/bit_stream.h"

#include <stdint.h>

class Logger;

class AISPosition {
    private:
        static constexpr uint8_t MINUTES_PER_DEGREE = 60;
        static constexpr int32_t LONGITUDE_UNKNOWN = 0x6791AC0;
        static constexpr int32_t LATITUDE_UNKNOWN = 0x3412140;

        int32_t longitudeTenThousandthsMinute;
        int32_t latitudeTenThousandthsMinute;
        // Temporary code
        float longitude() const;
        float latitude() const;
        float degreesToRadians(float degrees) const;

    public:
        AISPosition();
        AISPosition(etl::bit_stream_reader &streamReader);
        bool isValid() const;
        // Temporary code
        float distance(const AISPosition &other) const;

        friend Logger & operator << (Logger &logger, const AISPosition &position);
};

#endif // AIS_POSITION_H
