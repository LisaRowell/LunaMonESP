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

#ifndef NMEA_PARSER_H
#define NMEA_PARSER_H

#include "NMEATalker.h"
#include "NMEAMsgType.h"
#include "NMEAMessageBuffer.h"
#include "NMEADecapsulator.h"

#include "etl/bit_stream.h"

#include <stddef.h>

class AISContacts;
class NMEAMessage;
class NMEALine;

class NMEAParser {
    private:
        uint8_t nmeaMessageBuffer[NMEA_MESSAGE_BUFFER_SIZE];
        NMEADecapsulator decapsulator;
        AISContacts &aisContacts;

        NMEAMessage *parseUnencapsulatedLine(const NMEATalker &talker, const NMEAMsgType &msgType,
                                             NMEALine &nmeaLine);
        NMEAMessage *parseEncapsulatedLine(const NMEATalker &talker, const NMEAMsgType &msgType,
                                           NMEALine &nmeaLine);
        NMEAMessage *parseEncapsulatedMessage(const NMEATalker &talker,
                                              const NMEAMsgType &msgType);

    public:
        NMEAParser(AISContacts &aisContacts);
        NMEAMessage *parseLine(NMEALine &nmeaLine);
};

#endif // NMEA_PARSER_H
