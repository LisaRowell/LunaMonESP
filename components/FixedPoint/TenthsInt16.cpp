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

#include "TenthsInt16.h"
#include "TenthsUInt16.h"

#include "Logger.h"

#include "etl/string.h"
#include "etl/string_stream.h"

#include <stdint.h>

TenthsInt16::TenthsInt16() : _integer(0), _tenths(0) {
}

TenthsInt16::TenthsInt16(int16_t integer, uint8_t tenths)
    : _integer(integer), _tenths(tenths) {
}

int16_t TenthsInt16::integer() const {
    return _integer;
}

uint8_t TenthsInt16::tenths() const {
    return _tenths;
}

bool TenthsInt16::operator == (const TenthsInt16 &right) const {
    return _integer == right._integer && _tenths == right._tenths;
}

bool TenthsInt16::operator == (const TenthsUInt16 &right) const {
    return _integer == right.wholeNumber() && _tenths == right.tenths();
}

bool TenthsInt16::operator == (int16_t right) const {
    return _integer == right && _tenths == 0;
}

TenthsInt16 TenthsInt16::operator = (const TenthsUInt16 &right) {
    _integer = right.wholeNumber();
    _tenths = right.tenths();

    return *this;
}

TenthsInt16 TenthsInt16::operator = (int16_t right) {
    _integer = right;
    _tenths = 0;

    return *this;
}

void TenthsInt16::setFromTenths(int32_t tenths) {
    _integer = tenths / 10;
    _tenths = tenths % 10;
}

TenthsInt16 TenthsInt16::operator+(uint32_t adder) {
    return TenthsInt16(_integer + adder, _tenths);
}

TenthsInt16 TenthsInt16::operator*(uint32_t multiplier) {
    uint32_t tenthsMultiplied = _tenths * multiplier;
    uint32_t carry = tenthsMultiplied / 10;
    uint8_t tenths = tenthsMultiplied % 10;
    int16_t integer = _integer * multiplier + carry;
    return TenthsInt16(integer, tenths);
}

TenthsInt16 TenthsInt16::operator/(uint32_t divider) {
    int32_t integer = _integer / divider;
    uint32_t remainder = _integer % divider;
    uint32_t tenths = (remainder * 10 + _tenths) / divider;
    return TenthsInt16(integer, tenths);
}

TenthsUInt16 TenthsInt16::abs() const {
    uint16_t integer = _integer < 0 ? _integer * -1 : _integer;
    return TenthsUInt16(integer, _tenths);
}

bool TenthsInt16::operator <(int16_t value) {
    if (_integer < value) {
        return true;
    } else if (_integer > value) {
        return false;
    } else {
        if (_tenths == 0) {
            return false;
        } else {
            if (_integer < 0) {
                return true;
            } else {
                return false;
            }
        }
    }
}


void TenthsInt16::toString(etl::istring &string) const {
    etl::string_stream stringStream(string);

    stringStream << _integer << "." << _tenths;
}

// Doesn't honor base changes, but do they really makes sense for this type?
void TenthsInt16::log(Logger &logger) const {
    logger << _integer << "." << _tenths;
}
