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

#include "SeaTalkLine.h"

#include "Logger.h"

#include "etl/vector.h"
#include "etl/string.h"
#include "etl/to_string.h"
#include "etl/u16string.h"

#include <stdint.h>
#include <stddef.h>

SeaTalkLine::SeaTalkLine() : line(), overrun(false) {
}

void SeaTalkLine::append(uint8_t lineByte) {
    if (line.full()) {
        overrun = true;
    } else {
        line.push_back(lineByte);
    }
}

void SeaTalkLine::clear() {
    line.clear();
}

bool SeaTalkLine::isEmpty() const {
    return line.empty();
}

bool SeaTalkLine::isComplete() const {
    size_t currentLength = line.size();

    if (currentLength >= 3) {
        size_t expectedLength = (line[1] & 0x0f) + 3;
        return currentLength == expectedLength;
    } else {
        return false;
    }
}

bool SeaTalkLine::wasOverrun() const {
    return overrun;
}

size_t SeaTalkLine::length() const {
    return line.size();
}

uint8_t SeaTalkLine::operator [](size_t index) const {
    return line[index];
}

uint8_t SeaTalkLine::command() const {
    return line[0];
}

uint8_t SeaTalkLine::attribute() const {
    return line[1];
}

// Buffer must be at least maxLineLength words
size_t SeaTalkLine::buildNineBitString(uint16_t *buffer) const {
    size_t length;

    for (length = 0; length < line.size(); length++) {
        // The first character is the command byte which is encoded with its msb being set,
        // indicating the start of a datagram. Subsequent byte have the msb of 0.
        if (length == 0) {
            buffer[length] = (uint16_t)line[length] | 0x100;
        } else {
            buffer[length] = (uint16_t)line[length];
        }
    }

    return length;
}

Logger & operator << (Logger &logger, const SeaTalkLine &seaTalkLine) {
    logger << Hex;

    for (uint8_t byte : seaTalkLine.line) {
        etl::string<4> byteString;
        etl::format_spec byteFormat;
        byteFormat.hex().width(2).fill('0');
        etl::to_string(byte, byteString, byteFormat); 
        logger << byteString << " ";
    }

    return logger;
}
