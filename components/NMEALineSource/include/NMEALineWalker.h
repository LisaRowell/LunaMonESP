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

#ifndef NMEA_LINE_WALKER_H
#define NMEA_LINE_WALKER_H

#include "etl/string_view.h"

class NMEALine;

class NMEALineWalker {
    private:
        const NMEALine &nmeaLine;
        etl::string_view remaining;

    public:
        NMEALineWalker(const NMEALine &nmeaLine, bool stripStartAndEnd = false);
        void reset();
        // Returns true if the line is in the encapsulated encoding scheme AIS messages (and
        // possibly others), versus the normal style NMEA 0183 CSV data.
        bool isEncapsulatedData() const;
        void stripChecksum();
        bool getChar(char &character);
        bool getWord(etl::string_view &word);
        void skipChar();
        void skipWord();
        bool atEndOfLine() const;
};

#endif // NMEA_LINE_WALKER_H
