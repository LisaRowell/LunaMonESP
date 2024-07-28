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

#include "AISSpeedOverGround.h"

#include "Logger.h"

#include "etl/bit_stream.h"

#include <stdint.h>

AISSpeedOverGround::AISSpeedOverGround() {
    speedCode = SPEED_OVER_GROUND_NOT_AVAILABLE;
}

AISSpeedOverGround::AISSpeedOverGround(uint8_t speedKn) {
    speedCode = speedKn * 10;
}

AISSpeedOverGround::AISSpeedOverGround(etl::bit_stream_reader &streamReader) {
    speedCode = etl::read_unchecked<uint16_t>(streamReader, 10);
}

bool AISSpeedOverGround::isValid() const {
    return speedCode != SPEED_OVER_GROUND_NOT_AVAILABLE;
}

bool AISSpeedOverGround::isZero() const {
    return speedCode == 0;
}

Logger & operator << (Logger &logger, const AISSpeedOverGround &speedOverGround) {
    switch (speedOverGround.speedCode) {
        case AISSpeedOverGround::SPEED_OVER_GROUND_NOT_AVAILABLE:
            logger << "?";
            break;

        case AISSpeedOverGround::SPEED_OVER_GROUND_OVER_102_KN:
            logger << ">102";
            break;

        default:
            logger << speedOverGround.speedCode / 10 << "." << speedOverGround.speedCode % 10;
    }
    logger << "kn";

    return logger;
}
