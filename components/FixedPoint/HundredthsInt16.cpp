/*
 * This file is part of LunaMon (https://github.com/LisaRowell/LunaMon)
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

#include "HundredthsInt16.h"

#include "Logger.h"

HundredthsInt16::HundredthsInt16() : _integer(0), _hundredths(0) {
}

HundredthsInt16::HundredthsInt16(int16_t integer, uint8_t hundredths)
    : _integer(integer), _hundredths(hundredths) {
}

int16_t HundredthsInt16::integer() {
    return _integer;
}

uint8_t HundredthsInt16::hundredths() {
    return _hundredths;
}

void HundredthsInt16::setFromHundredths(int32_t hundredths) {
    _integer = hundredths / 100;
    _hundredths = hundredths % 100;
}

HundredthsInt16 HundredthsInt16::operator+(uint32_t adder) {
    return HundredthsInt16(_integer + adder, _hundredths);
}

HundredthsInt16 HundredthsInt16::operator*(uint32_t multiplier) {
    uint32_t hundredthsMultiplied = _hundredths * multiplier;
    uint32_t carry = hundredthsMultiplied / 100;
    uint8_t hundredths = hundredthsMultiplied % 100;
    int16_t integer = _integer * multiplier + carry;
    return HundredthsInt16(integer, hundredths);
}

HundredthsInt16 HundredthsInt16::operator/(uint32_t divider) {
    int32_t integer = _integer / divider;
    uint32_t remainder = _integer % divider;
    uint32_t hundredths = (remainder * 100 + _hundredths) / divider;
    return HundredthsInt16(integer, hundredths);
}

// Doesn't honor base changes, but do they really makes sense for this type?
void HundredthsInt16::log(Logger &logger) const {
    logger << _integer << "." << _hundredths / 10 << _hundredths % 10;
}
