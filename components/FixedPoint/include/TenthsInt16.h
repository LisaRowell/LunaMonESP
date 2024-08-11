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

#ifndef TENTHS_INT16_H
#define TENTHS_INT16_H

#include "LoggableItem.h"

#include "etl/string.h"

#include <stdint.h>

class TenthsUInt16;

class TenthsInt16 : public LoggableItem {
    private:
        int16_t _integer;
        uint8_t _tenths;

    public:
        TenthsInt16();
        TenthsInt16(int16_t integer, uint8_t tenths);
        int16_t integer() const;
        uint8_t tenths() const;
        bool operator == (const TenthsInt16 &right) const;
        bool operator == (const TenthsUInt16 &right) const;
        bool operator == (int16_t right) const;
        TenthsInt16 operator = (const TenthsUInt16 &right);
        TenthsInt16 operator = (int16_t right);
        void setFromTenths(int32_t tenths);
        TenthsInt16 operator+(uint32_t adder);
        TenthsInt16 operator*(uint32_t multiplier);
        TenthsInt16 operator/(uint32_t divider);
        TenthsUInt16 abs() const;
        bool operator <(int16_t value);
        void toString(etl::istring &string) const;
        virtual void log(Logger &logger) const override;
};

#endif // TENTHS_INT16_H
