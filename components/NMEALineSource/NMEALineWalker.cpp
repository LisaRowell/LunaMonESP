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

#include "NMEALineWalker.h"
#include "NMEALine.h"

#include "etl/string.h"
#include "etl/string_view.h"

NMEALineWalker::NMEALineWalker(const NMEALine &nmeaLine, bool stripStartAndEnd)
    : nmeaLine(nmeaLine) {
    reset();
    if (stripStartAndEnd) {
        skipChar();
        stripChecksum();
    }
}

void NMEALineWalker::reset() {
    remaining = nmeaLine.contents();
}

bool NMEALineWalker::isEncapsulatedData() const {
    return nmeaLine.contents().front() == '!';
}

void NMEALineWalker::stripChecksum() {
    remaining.remove_suffix(3);
}

bool NMEALineWalker::getChar(char &character) {
    if (remaining.empty()) {
        return false;
    }

    character = remaining.front();
    remaining.remove_prefix(1);

    return true;
}

bool NMEALineWalker::getWord(etl::string_view &word) {
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

void NMEALineWalker::skipChar() {
    remaining.remove_prefix(1);
}

void NMEALineWalker::skipWord() {
    size_t commaPos = remaining.find(',');
    if (commaPos == remaining.npos) {
        remaining.remove_prefix(remaining.size());
    } else {
        remaining.remove_prefix(commaPos + 1);
    }
}

bool NMEALineWalker::atEndOfLine() const {
    return remaining.empty();
}
