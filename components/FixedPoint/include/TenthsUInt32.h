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

#ifndef TENTHS_UINT32_H
#define TENTHS_UINT32_H

#include "LoggableItem.h"

#include "etl/string.h"

#include <stdint.h>

class TenthsUInt32 : public LoggableItem {
    private:
        uint32_t _wholeNumber;
        uint8_t _tenths;

    public:
        TenthsUInt32();
        TenthsUInt32(uint32_t wholeNumber, uint8_t tenths);
        uint32_t wholeNumber() const;
        uint8_t tenths() const;
        bool operator == (const TenthsUInt32 &right) const;
        bool operator == (uint32_t right) const;
        TenthsUInt32 operator = (uint32_t right);
        void setFromTenths(uint32_t tenths);
        TenthsUInt32 operator+(uint32_t adder);
        TenthsUInt32 operator+(const TenthsUInt32 &other) const;
        void toString(etl::istring &string) const;
        virtual void log(Logger &logger) const override;
};

#endif // TENTHS_UINT32_H
