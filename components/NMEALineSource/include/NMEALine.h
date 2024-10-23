/*
 * This file is part of LunaMon (https://github.com/LisaRowell/LunaMonESP)
 * Copyright (C) 2021-2024 Lisa Rowell
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

#ifndef NMEA_LINE_H
#define NMEA_LINE_H

#include "etl/string.h"
#include "etl/string_view.h"

#include <stddef.h>

class Logger;

const size_t maxNMEALineLength = 82;

class NMEALine {
    private:
        etl::string<maxNMEALineLength> line;

        bool validateChecksum();

    public:
        NMEALine();
        NMEALine(const etl::istring &string);
        void reset();
        void append(const char *srcBuffer, size_t start, size_t end);
        void append(const etl::istring &string);
        void append(const char *string);
        void append(char character);
        bool isEmpty();
        bool sanityCheck();
        void appendWord(const etl::istring &string);
        void appendChecksum();
        const etl::istring &contents() const;
        const char *data() const;
        size_t length() const;

    friend Logger & operator << (Logger &logger, const NMEALine &nmeaLine);
};

#endif // NMEA_LINE_H
