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

#include "AISDimensions.h"

#include "Logger.h"

#include "etl/bit_stream.h"

#include <stdint.h>

AISDimensions::AISDimensions() : _lengthM(0), _widthM(0) {
}

AISDimensions::AISDimensions(etl::bit_stream_reader &streamReader) {
    set(streamReader);
}

void AISDimensions::set(etl::bit_stream_reader &streamReader) {
    // Per gitlab:
    //    Ship dimensions will be 0 if not available. For the dimensions to bow and stern, the
    //    special value 511 indicates 511 meters or greater; for the dimensions to port and
    //    starboard, the special value 63 indicates 63 meters or greater.

    uint16_t dimensionToBow = etl::read_unchecked<uint16_t>(streamReader, 9);
    uint16_t dimensionToStern = etl::read_unchecked<uint16_t>(streamReader, 9);
    if (dimensionToBow == 0 || dimensionToStern == 0) {
        _lengthM = 0;
    } else if (dimensionToBow == LARGE_VESSEL_LENGTH || dimensionToStern == LARGE_VESSEL_LENGTH) {
        _lengthM = LARGE_VESSEL_LENGTH;
    } else {
        _lengthM = dimensionToBow + dimensionToStern;
    }

    uint8_t dimensionToPort = etl::read_unchecked<uint8_t>(streamReader, 6);
    uint8_t dimensionToStarboard = etl::read_unchecked<uint8_t>(streamReader, 6);
    if (dimensionToPort == 0 || dimensionToStarboard == 0) {
        _widthM = 0;
    } else if (dimensionToPort == LARGE_VESSEL_WIDTH ||
               dimensionToStarboard == LARGE_VESSEL_WIDTH) {
        _widthM = LARGE_VESSEL_WIDTH;
    } else {
        _widthM = dimensionToPort + dimensionToStarboard;
    }
}

bool AISDimensions::isSet() const {
    return _lengthM != 0 && _widthM != 0;
}

uint16_t AISDimensions::lengthM() const {
    return _lengthM;
}

uint8_t AISDimensions::widthM() const {
    return _widthM;
}

Logger & operator << (Logger &logger, const AISDimensions &dimensions) {
    if (dimensions._lengthM == 0) {
        logger << "?";
    } else if (dimensions._lengthM == dimensions.LARGE_VESSEL_LENGTH) {
        logger << ">=" << dimensions.LARGE_VESSEL_LENGTH << "m";
    } else {
        logger << dimensions._lengthM << "m";
    }

    logger << " x ";

    if (dimensions._widthM == 0) {
        logger << "?";
    } else if (dimensions._widthM == dimensions.LARGE_VESSEL_WIDTH) {
        logger << ">=" << dimensions.LARGE_VESSEL_WIDTH << "m";
    } else {
        logger << dimensions._widthM << "m";
    }

    return logger;
}
