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

#ifndef NMEA_CLIENT_H
#define NMEA_CLIENT_H

#include <lwip/sockets.h>

class NMEALine;
class Logger;
class DataModelStringLeaf;

class NMEAClient {
    private:
        int socket;
        struct sockaddr_in sourceAddr;

    public:
        NMEAClient(int socket, struct sockaddr_in &sourceAddr);
        bool sendLine(const NMEALine &inputLine, bool &dropped);
        void setNameLeaf(DataModelStringLeaf *nameLeaf);
        ~NMEAClient();

    friend Logger & operator << (Logger &logger, const NMEAClient &client);
};

#endif // NMEA_CLIENT_H
