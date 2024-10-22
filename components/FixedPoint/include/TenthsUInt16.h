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

#ifndef TENTHS_UINT16_H
#define TENTHS_UINT16_H

#include "LoggableItem.h"

#include "etl/string.h"

#include <stdint.h>

class TenthsInt16;
class HundredthsUInt16;

class TenthsUInt16 : public LoggableItem {
    private:
        uint16_t _wholeNumber;
        uint8_t _tenths;

    public:
        TenthsUInt16();
        TenthsUInt16(uint16_t wholeNumber, uint8_t tenths);
        uint16_t wholeNumber() const;
        uint8_t tenths() const;
        bool operator == (const TenthsUInt16 &right) const;
        bool operator == (uint16_t right) const;
        TenthsUInt16 operator = (uint16_t right);
        void setFromTenths(uint32_t tenths);
        void roundFrom(const HundredthsUInt16 &other);
        TenthsUInt16 operator+(uint16_t adder);
        TenthsUInt16 operator+(const TenthsUInt16 &other) const;
        TenthsInt16 operator-(const TenthsUInt16 &other) const;
        TenthsUInt16 operator*(uint32_t multiplier);
        TenthsUInt16 operator/(uint32_t divider);
        void toString(etl::istring &string) const;
        virtual void log(Logger &logger) const override;
};

#endif // TENTHS_UINT16_H
