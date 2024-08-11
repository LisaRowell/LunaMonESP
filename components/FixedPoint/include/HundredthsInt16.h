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

#ifndef HUNDREDTHS_INT16_H
#define HUNDREDTHS_INT16_H

#include "LoggableItem.h"

#include "etl/string.h"

#include <stdint.h>

class HundredthsInt16 : public LoggableItem {
    private:
        int16_t _integer;
        uint8_t _hundredths;

    public:
        HundredthsInt16();
        HundredthsInt16(int16_t integer, uint8_t hundredths);
        int16_t integer() const;
        uint8_t hundredths() const;
        bool operator == (const HundredthsInt16 &right) const;
        void setFromHundredths(int32_t hundredths);
        HundredthsInt16 operator+(uint32_t adder);
        HundredthsInt16 operator*(uint32_t multiplier);
        HundredthsInt16 operator/(uint32_t divider);
        void toString(etl::istring &string) const;
        virtual void log(Logger &logger) const override;
};

#endif // HUNDREDTHS_INT16_H
