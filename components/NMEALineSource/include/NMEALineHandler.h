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

#ifndef NMEA_LINE_HANDLER_H
#define NMEA_LINE_HANDLER_H

class NMEALine;
class NMEATalker;
class NMEAMsgType;

class NMEALineHandler {
    public:
        virtual void handleLine(const NMEALine &inputLine, const NMEATalker &talker,
                                const NMEAMsgType &msgType) = 0;
};


#endif // NMEA_LINE_HANDLER_H
