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

#include "TenthsUInt32.h"
#include "TenthsUInt16.h"

#include "Logger.h"

#include "etl/string.h"
#include "etl/string_stream.h"

#include <stdint.h>

TenthsUInt32::TenthsUInt32() : _wholeNumber(0), _tenths(0) {
}

TenthsUInt32::TenthsUInt32(uint32_t wholeNumber, uint8_t tenths)
    : _wholeNumber(wholeNumber),
      _tenths(tenths) {
}

TenthsUInt32::TenthsUInt32(const TenthsUInt16 &other)
    : _wholeNumber(other.wholeNumber()),
      _tenths(other.tenths()) {
}

uint32_t TenthsUInt32::wholeNumber() const {
    return _wholeNumber;
}

uint8_t TenthsUInt32::tenths() const {
    return _tenths;
}

bool TenthsUInt32::operator == (const TenthsUInt32 &right) const {
    return _wholeNumber == right._wholeNumber && _tenths == right._tenths;
}

bool TenthsUInt32::operator == (uint32_t right) const {
    return _wholeNumber == right && _tenths == 0;
}

TenthsUInt32 TenthsUInt32::operator = (uint32_t right) {
    _wholeNumber = right;
    _tenths = 0;

    return *this;
}

void TenthsUInt32::setFromTenths(uint32_t tenths) {
    _wholeNumber = tenths / 10;
    _tenths = tenths % 10;
}

TenthsUInt32 TenthsUInt32::operator+(uint32_t adder) {
    return TenthsUInt32(_wholeNumber + adder, _tenths);
}

TenthsUInt32 TenthsUInt32::operator+(const TenthsUInt32 &other) const {
    uint8_t tenths = _tenths + other._tenths;
    uint32_t wholeNumber = _wholeNumber + other._wholeNumber;

    if (tenths > 10) {
        tenths -= 10;
        wholeNumber += 1;
    }

    return TenthsUInt32(wholeNumber, tenths);
}

TenthsUInt32 TenthsUInt32::operator*(uint32_t multiplier) const {
    uint32_t tenthsMultiplied = _tenths * multiplier;
    uint32_t carry = tenthsMultiplied / 10;
    uint8_t tenths = tenthsMultiplied % 10;
    uint32_t wholeNumber = _wholeNumber * multiplier + carry;
    return TenthsUInt32(wholeNumber, tenths);
}

TenthsUInt32 TenthsUInt32::operator/(uint32_t divider) const {
    uint32_t wholeNumber = _wholeNumber / divider;
    uint32_t remainder = _wholeNumber % divider;
    uint32_t tenths = (remainder * 10 + _tenths) / divider;
    return TenthsUInt32(wholeNumber, tenths);
}

void TenthsUInt32::toString(etl::istring &string) const {
    etl::string_stream stringStream(string);

    stringStream << _wholeNumber << "." << _tenths;
}

// Doesn't honor base changes, but do they really makes sense for this type?
void TenthsUInt32::log(Logger &logger) const {
    logger << _wholeNumber << "." << _tenths;
}
