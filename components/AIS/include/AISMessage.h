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

#ifndef AIS_MESSAGE_H
#define AIS_MESSAGE_H

#include "AISMsgType.h"

#include "etl/bit_stream.h"

#include <stddef.h>

class AISContacts;
class AISMMSI;

class AISMessage {
    private:
        AISMsgType msgType;

        bool parseCommonNavigationBlock(etl::bit_stream_reader &streamReader,
                                        size_t messageSizeInBits, bool ownShip,
                                        AISContacts &aisContacts);
        bool parseStaticAndVoyageRelatedData(etl::bit_stream_reader &streamReader,
                                             size_t messageSizeInBits, bool ownShip,
                                             AISContacts &aisContacts);
        bool parseStandardClassBPositionReport(etl::bit_stream_reader &streamReader,
                                               size_t messageSizeInBits, bool ownShip,
                                               AISContacts &aisContacts);
        bool parseStaticDataReport(etl::bit_stream_reader &streamReader, size_t messageSizeInBits,
                                   bool ownShip, AISContacts &aisContacts);
        void parseStaticDataReportPartA(etl::bit_stream_reader &streamReader,
                                        size_t messageSizeInBits, bool ownShip,
                                        AISContacts &aisContacts, uint8_t repeatIndicator,
                                        AISMMSI &mmsi);
        void parseStaticDataReportPartB(etl::bit_stream_reader &streamReader,
                                        size_t messageSizeInBits, bool ownShip,
                                        AISContacts &aisContacts, uint8_t repeatIndicator,
                                        AISMMSI &mmsi);
        void createContactError(AISMMSI &mmsi);

    public:
        bool parse(etl::bit_stream_reader &streamReader, size_t messageSizeInBits, bool ownShip,
                   AISContacts &aisContacts);
        void log(const char *nmeaMsgTypeName) const;
};

#endif // AIS_MESSAGE_H
