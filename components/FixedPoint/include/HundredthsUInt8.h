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

#ifndef HUNDREDTHS_UINT8_H
#define HUNDREDTHS_UINT8_H

#include "LoggableItem.h"

#include "etl/string.h"

#include <stdint.h>

class HundredthsUInt8 : public LoggableItem {
    private:
        uint8_t _wholeNumber;
        uint8_t _hundredths;

    public:
        HundredthsUInt8();
        HundredthsUInt8(uint8_t wholeNumber, uint8_t hundredths);
        uint8_t wholeNumber() const;
        uint8_t hundredths() const;
        HundredthsUInt8 & operator = (uint8_t value);
        bool operator == (const HundredthsUInt8 &right) const;
        bool operator == (uint8_t right) const;
        void setFromQ22Dot10(uint32_t q22Dot10);
        void toString(etl::istring &string) const;
        virtual void log(Logger &logger) const override;
};

#endif // HUNDREDTHS_UINT8_H
