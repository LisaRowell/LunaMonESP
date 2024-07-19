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

#include "AISString.h"

#include "Logger.h"

#include "etl/string.h"
#include "etl/bit_stream.h"

#include <stddef.h>

// Note that the size of the buffer must by at least maxLength+1 bytes.
AISString::AISString(char *buffer, size_t maxLength) : string(buffer, maxLength + 1) {
    string.clear();
}

AISString::AISString(char *buffer, size_t length, etl::bit_stream_reader &streamReader)
    : string(buffer, length + 1) {
    string.clear();
    append(length, streamReader);
}

void AISString::append(size_t length, etl::bit_stream_reader &streamReader) {
    while (length--) {
        const char sixBitCode = etl::read_unchecked<char>(streamReader, 6);
        const char character = codeToChar(sixBitCode);
        if (character == '@') {
            streamReader.skip(length * 6);
            break;
        }
        string.push_back(character);
    }
}

void AISString::removeTrailingBlanks() {
    while (!string.empty() && string.back() == ' ') {
        string.pop_back();
    }
}

AISString & AISString::operator = (const AISString &other) {
    string.assign(other.string);

    return *this;
}

char AISString::codeToChar(char sixBitCode) const {
    if (sixBitCode < 32) {
        return sixBitCode + '@';
    } else {
        return (sixBitCode - 32) + ' ';
    }
}

Logger & operator << (Logger &logger, const AISString &aisString) {
    logger << aisString.string;

    return logger;
}
