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

#include "NMEAMMSI.h"

#include "Logger.h"

#include "etl/bit_stream.h"
#include "etl/string.h"

#include <stddef.h>

void NMEAMMSI::parse(etl::bit_stream_reader &streamReader) {
    mmsi = etl::read_unchecked<uint32_t>(streamReader, 30);
}

void NMEAMMSI::log(Logger &logger) const {
    logger << mmsi;
}
