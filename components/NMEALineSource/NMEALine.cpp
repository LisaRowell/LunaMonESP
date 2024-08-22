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

NMEALine::NMEALine() : line(), remaining(){
}

void NMEALine::reset() {
    line.clear();
    remaining = line;
}

void NMEALine::append(const char *srcBuffer, size_t start, size_t end) {
    line.append(srcBuffer + start, end - start);
    if (line.is_truncated()) {
        logger() << logWarnNMEALine << "NMEA line exceeded maximum length, truncated" << eol;
    }
    remaining = line;
}

void NMEALine::append(const etl::istring &string) {
    line.append(string);
    if (line.is_truncated()) {
        logger() << logWarnNMEALine << "NMEA line exceeded maximum length, truncated" << eol;
    }

    remaining = line;
}

void NMEALine::append(const char *string) {
    line.append(string);
    if (line.is_truncated()) {
        logger() << logWarnNMEALine << "NMEA line exceeded maximum length, truncated" << eol;
    }

    remaining = line;
}

void NMEALine::append(char character) {
    line.push_back(character);
    if (line.is_truncated()) {
        logger() << logWarnNMEALine << "NMEA line exceeded maximum length, truncated" << eol;
    }

    remaining = line;
}

bool NMEALine::isEmpty() {
    return line.empty();
}

bool NMEALine::isEncapsulatedData() {
    return encapsulatedData;
}

bool NMEALine::sanityCheck() {
    char msgStart;

    if (!extractChar(msgStart)) {
        logger() << logWarnNMEALine << "Empty NMEA message" << eol;
        return false;
    }

    switch (msgStart) {
        case '$':
            encapsulatedData = false;
            break;

        case '!':
            encapsulatedData = true;
            break;

        default:
            logger() << logWarnNMEALine << "NMEA message missing leading '$'" << eol;
            logLine();
            return false;
    }

    if (!checkParity()) {
        logger() << logWarnNMEALine << "NMEA line with bad parity: " << line << eol;
        return false;
    }
    stripParity();

    return true;
}

bool NMEALine::extractChar(char &character) {
    if (remaining.empty()) {
        return false;
    }

    character = remaining.front();
    remaining.remove_prefix(1);

    return true;
}

bool NMEALine::getWord(etl::string_view &word) {
    if (remaining.empty()) {
        return false;
    }

    size_t commaPos = remaining.find(',');
    if (commaPos == remaining.npos) {
        word.assign(remaining.begin(), remaining.end());
        remaining.remove_prefix(remaining.size());
    } else {
        word.assign(remaining.begin(), remaining.begin() + commaPos);
        remaining.remove_prefix(commaPos + 1);
    }

    return true;
}

void NMEALine::appendWord(const etl::istring &string) {
    line.push_back(',');
    line.append(string);
    remaining = line;
}

void NMEALine::appendParity() {
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

bool NMEALine::atEndOfLine() {
    return remaining.empty();
}

void NMEALine::stripParity() {
    remaining.remove_suffix(3);
}

bool NMEALine::checkParity() {
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

void NMEALine::logLine() {
    logger() << logDebugNMEALine << line << eol;
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
