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

#ifndef HUNDREDTHS_UINT32_H
#define HUNDREDTHS_UINT32_H

#include <stdint.h>

#include "LoggableItem.h"

#include "etl/string.h"

class HundredthsUInt32 : public LoggableItem {
    private:
        uint32_t _wholeNumber;
        uint8_t _hundredths;

    public:
        HundredthsUInt32();
        HundredthsUInt32(uint32_t wholeNumber, uint8_t hundredths);
        uint32_t wholeNumber();
        uint8_t hundredths();
        bool operator == (const HundredthsUInt32 &right) const;
        void setFromQ22Dot10(uint32_t q22Dot10);
        void setFromQ24Dot8(uint32_t q24Dot8);
        HundredthsUInt32 operator/(uint32_t divider);
        void toString(etl::istring &string) const;
        virtual void log(Logger &logger) const override;
};

#endif  // HUNDREDTHS_UINT32_H
