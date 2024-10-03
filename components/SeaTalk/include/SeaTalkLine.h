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

#ifndef SEATALK_LINE_H
#define SEATALK_LINE_H

#include "etl/vector.h"
#include "etl/u16string.h"

#include <stdint.h>
#include <stddef.h>

class Logger;

class SeaTalkLine {
    public:
        static constexpr size_t maxLineLength = 20; // Just a guess

    private:
        etl::vector<uint8_t, maxLineLength> line;
        bool overrun;

    public:
        SeaTalkLine();
        void append(uint8_t messageByte);
        void clear();
        bool isEmpty() const;
        bool isComplete() const;
        bool wasOverrun() const;
        size_t length() const;
        uint8_t operator [](size_t index) const;
        uint8_t command() const;
        uint8_t attribute() const;
        size_t buildNineBitString(uint16_t *buffer) const;

    friend Logger & operator << (Logger &logger, const SeaTalkLine &seaTalkLine);
};

#endif // SEATALK_LINE_H
