/*
 * This file is part of LunaMon (https://github.com/LisaRowell/LunaMonESP)
 * Copyright (C) 2023 Lisa Rowell
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

#include "HundredthsUInt8.h"

#include "Logger.h"

HundredthsUInt8::HundredthsUInt8() : _wholeNumber(0), _hundredths(0) {
}

HundredthsUInt8::HundredthsUInt8(uint8_t wholeNumber, uint8_t hundredths)
    : _wholeNumber(wholeNumber), _hundredths(hundredths) {
}

uint8_t HundredthsUInt8::wholeNumber() {
    return _wholeNumber;
}

uint8_t HundredthsUInt8::hundredths() {
    return _hundredths;
}

void HundredthsUInt8::setFromQ22Dot10(uint32_t q22Dot10) {
    _wholeNumber = q22Dot10 / 1024;
    uint16_t fractional1024ths = q22Dot10 % 1024;
    _hundredths = (fractional1024ths * 100) / 1024;
}

// Doesn't honor base changes, but do they really makes sense for this type?
void HundredthsUInt8::log(Logger &logger) const {
    logger << _wholeNumber << "." << _hundredths / 10 << _hundredths % 10;
}
