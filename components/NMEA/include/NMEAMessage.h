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

#ifndef NMEA_MESSAGE_H
#define NMEA_MESSAGE_H

#include "NMEATalker.h"
#include "NMEAMsgType.h"

class NMEALine;

class NMEAMessage {
    protected:
        NMEATalker talker;

        bool extractConstantWord(NMEALine &nmeaLine, const char *messageType,
                                 const char *constantWord);

    public:
        NMEAMessage(const NMEATalker &talker);
        const NMEATalker &source() const;
        virtual NMEAMsgType::Value type() const = 0;
        const char *typeName() const;
        virtual void log() const = 0;
};

NMEAMessage *parseNMEAMessage(NMEALine &nmeaLine);

#endif
