/*
 * This file is part of LunaMon (https://github.com/LisaRowell/LunaMonESP)
 * Copyright (C) 2021-2024 Lisa Rowell
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

#include "NMEALine.h"

#include "CharacterTools.h"
#include "Logger.h"

#include "etl/string.h"
#include "etl/string_view.h"
#include "etl/to_string.h"

#include <stddef.h>

NMEALine::NMEALine() : line() {
}

NMEALine::NMEALine(const etl::istring &string) : line(string) {
}

void NMEALine::reset() {
    line.clear();
}

void NMEALine::append(const char *srcBuffer, size_t start, size_t end) {
    line.append(srcBuffer + start, end - start);
    if (line.is_truncated()) {
        logger() << logWarnNMEALine << "NMEA line exceeded maximum length, truncated" << eol;
    }
}

void NMEALine::append(const etl::istring &string) {
    line.append(string);
    if (line.is_truncated()) {
        logger() << logWarnNMEALine << "NMEA line exceeded maximum length, truncated" << eol;
    }
}

void NMEALine::append(const char *string) {
    line.append(string);
    if (line.is_truncated()) {
        logger() << logWarnNMEALine << "NMEA line exceeded maximum length, truncated" << eol;
    }
}

void NMEALine::append(char character) {
    line.push_back(character);
    if (line.is_truncated()) {
        logger() << logWarnNMEALine << "NMEA line exceeded maximum length, truncated" << eol;
    }
}

bool NMEALine::isEmpty() {
    return line.empty();
}

bool NMEALine::sanityCheck() {
    if (isEmpty()) {
        logger() << logWarnNMEALine << "Empty NMEA message" << eol;
        return false;
    }

    char firstCharacter = line.front();
    if (firstCharacter != '$' && firstCharacter != '!') {
        logger() << logWarnNMEALine << "NMEA message missing leading '$' or '!': " << line << eol;
        return false;
    }

    if (!validateChecksum()) {
        logger() << logWarnNMEALine << "NMEA line with bad checksum: " << line << eol;
        return false;
    }

    return true;
}

void NMEALine::appendWord(const etl::istring &string) {
    line.push_back(',');
    line.append(string);
}

void NMEALine::appendChecksum() {
    line.push_back('*');

    // Parity is calulated as being over all characters in the message between the opening '$'
    // (or '!') and the '*', non-inclusive.
    uint8_t checksum = 0;
    for (int pos = 1; pos < line.length() - 1; pos++) {
        checksum ^= line[pos];
    }

    etl::string<3> checksumString;
    etl::format_spec checksumFormat;
    checksumFormat.hex().upper_case(true).width(2).fill('0');
    etl::to_string(checksum, checksumString, checksumFormat);

    line.append(checksumString);
}

bool NMEALine::validateChecksum() {
    size_t checksumPos = line.size() - 3;
    if (line[checksumPos] != '*') {
        return false;
    }

    uint8_t checksum = 0;
    for (size_t pos = 1; pos < checksumPos; pos++) {
        checksum = checksum ^ line[pos];
    }

    const uint8_t firstChecksumChar = line[checksumPos + 1];
    const uint8_t secondChecksumChar = line[checksumPos + 2];
    if (!isUpperCaseHexidecimalDigit(firstChecksumChar) ||
        !isUpperCaseHexidecimalDigit(secondChecksumChar)) {
        return false;
    }
    const uint8_t lineChecksum = hexidecimalValue(firstChecksumChar) * 16 +
                                 hexidecimalValue(secondChecksumChar);

    return lineChecksum == checksum;
}

const etl::istring &NMEALine::contents() const {
    return line;
}

const char *NMEALine::data() const {
    return line.data();
}

size_t NMEALine::length() const {
    return line.size();
}

Logger & operator << (Logger &logger, const NMEALine &nmeaLine) {
    // Depending upon where this is done, the line may or may not have a CRLF at the end. If it
    // does, avoid logging it.
    etl::string_view lineView(nmeaLine.line);
    if (lineView.back() == '\n') {
        lineView.remove_suffix(1);
        if (lineView.back() == '\r') {
            lineView.remove_suffix(1);
        }
    }

    logger << lineView;

    return logger;
}
