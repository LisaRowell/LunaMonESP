/*
 * This file is part of LunaMon (https://github.com/LisaRowell/LunaMonESP)
 * Copyright (C) 2023-2024 Lisa Rowell
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

#include "HundredthsUInt32.h"

#include "Logger.h"

#include "etl/string.h"
#include "etl/string_stream.h"

HundredthsUInt32::HundredthsUInt32() : _wholeNumber(0), _hundredths(0) {
}

HundredthsUInt32::HundredthsUInt32(uint32_t wholeNumber, uint8_t hundredths)
    : _wholeNumber(wholeNumber), _hundredths(hundredths) {
}

uint32_t HundredthsUInt32::wholeNumber() {
    return _wholeNumber;
}

uint8_t HundredthsUInt32::hundredths() {
    return _hundredths;
}

bool HundredthsUInt32::operator == (const HundredthsUInt32 &right) const {
    return _wholeNumber == right._wholeNumber && _hundredths == right._hundredths;
}

void HundredthsUInt32::setFromQ22Dot10(uint32_t q22Dot10) {
    _wholeNumber = q22Dot10 / 1024;
    uint16_t fractional1024ths = q22Dot10 % 1024;
    _hundredths = (fractional1024ths * 100) / 1024;
}

void HundredthsUInt32::setFromQ24Dot8(uint32_t q24Dot8) {
    _wholeNumber = q24Dot8 / 256;
    uint16_t fractional256ths = q24Dot8 % 256;
    _hundredths = (fractional256ths * 100) / 256;
}

HundredthsUInt32 HundredthsUInt32::operator/ (uint32_t divider) {
    uint32_t newWholeNumber = _wholeNumber / divider;
    uint32_t remainder = _wholeNumber % divider;
    uint32_t newHundredths = (remainder * 100 + _hundredths) / divider;
    return HundredthsUInt32(newWholeNumber, newHundredths);
}

void HundredthsUInt32::toString(etl::istring &string) const {
    etl::string_stream stringStream(string);

    stringStream << _wholeNumber << "." << etl::setfill(0) << etl::setw(2) << _hundredths;
}

// Doesn't honor base changes, but do they really makes sense for this type?
void HundredthsUInt32::log(Logger &logger) const {
    logger << _wholeNumber << "." << _hundredths / 10 << _hundredths % 10;
}
