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

#include "NMEAAISName.h"

#include "Logger.h"

#include "etl/string.h"
#include "etl/bit_stream.h"

NMEAAISName::NMEAAISName() : name("Unset") {
}

void NMEAAISName::parse(etl::bit_stream_reader &streamReader) {
    name.clear();

    for (unsigned characters = MAX_NAME_BASE_SIZE; characters; characters--) {
        const char sixBitCode = etl::read_unchecked<char>(streamReader, 6);
        const char character = codeToChar(sixBitCode);
        if (character == '@') {
            break;
        }
        name.push_back(character);
    }

    // In the wild, stations are actually not using the '@' terminator and are instead padding the
    // field out with spaces. Strip trailing spaces...
    while (name.length() && name.back() == ' ') {
        name.pop_back();
    }
}

void NMEAAISName::parseExtension(etl::bit_stream_reader &streamReader, uint8_t characters) {
    while (characters--) {
        const char sixBitCode = etl::read_unchecked<char>(streamReader, 6);
        const char character = codeToChar(sixBitCode);

        // Documentation found on the topic says that the name extension shouldn't contain '@'
        // terminators, but in reality it seems to be common for extension fields to just contain
        // a single '@' and nothing else.
        if (character == '@') {
            break;
        }
        name.push_back(character);
    }
}

char NMEAAISName::codeToChar(char sixBitCode) const {
    if (sixBitCode < 32) {
        return sixBitCode + '@';
    } else {
        return (sixBitCode - 32) + ' ';
    }
}

void NMEAAISName::log(Logger &logger) const {
    logger << name;
}
