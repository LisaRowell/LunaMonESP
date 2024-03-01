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

#ifndef HUNDREDTHS_UINT16_H
#define HUNDREDTHS_UINT16_H

#include "LoggableItem.h"

#include "etl/string.h"

#include <stdint.h>

class HundredthsUInt16 : public LoggableItem {
    private:
        uint16_t _wholeNumber;
        uint8_t _hundredths;

    public:
        HundredthsUInt16();
        HundredthsUInt16(uint16_t wholeNumber, uint8_t hundredths);
        uint16_t wholeNumber();
        uint8_t hundredths();
        bool operator == (const HundredthsUInt16 &right) const;
        void setFromQ22Dot10(uint32_t q22Dot10);
        void toString(etl::istring &string) const;
        virtual void log(Logger &logger) const override;
};

#endif // HUNDREDTHS_UINT16_H
