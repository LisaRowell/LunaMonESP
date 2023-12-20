/*
 * This file is part of LunaMon (https://github.com/LisaRowell/LunaMonESP)
 * Copyright (C) 2021-2023 Lisa Rowell
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

#include "CharacterTools.h"

#include <stdint.h>
#include <ctype.h>

uint8_t decimalValue(char character) {
    return character - '0';
}

bool isUpperCaseHexidecimalDigit(char character) {
    return isdigit(character) || (isupper(character) && isxdigit(character));
}

uint8_t hexidecimalValue(char character) {
    if (isdigit(character)) {
        return decimalValue(character);
    } else {
        if (isupper(character)) {
            return character - 'A' + 10;
        } else {
            return character - 'a' + 10;
        }
    }
}

bool isCarriageReturn(char character) {
    return character == '\r';
}

bool isLineFeed(char character) {
    return character == '\n';
}
