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

#ifndef NMEA_LINE_SOURCE_H
#define NMEA_LINE_SOURCE_H

#include "NMEALine.h"
#include "NMEALineHandler.h"

#include "etl/vector.h"

#include <stddef.h>

class NMEALineSource {
    private:
        static constexpr size_t MAX_LINE_HANDLERS = 4;

        etl::vector<NMEALineHandler *, MAX_LINE_HANDLERS> lineHandlers;
        NMEALine inputLine;
        bool carriageReturnFound;

        bool scanForCarriageReturn(size_t &carriageReturnPos, const char *buffer,
                                   size_t &bufferPos, size_t &remaining);
        bool processBufferToEndOfLine(const char *buffer, size_t &bufferPos, size_t &remaining);
        void lineCompleted();
        void handleLine(NMEALine &inputLine);

    protected:
        void processBuffer(const char *buffer, size_t length);

    public:
        NMEALineSource();
        void addLineHandler(NMEALineHandler &lineHandler);
        void sourceReset();
};

#endif // NMEA_LINE_SOURCE_H
