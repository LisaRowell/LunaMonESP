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

#ifndef NMEA_WIFI_SOURCE_H
#define NMEA_WIFI_SOURCE_H

#include "TaskObject.h"
#include "WiFiManagerClient.h"
#include "NMEASockSource.h"

#include "DataModelNode.h"
#include "DataModelUInt32Leaf.h"
#include "DataModelBoolLeaf.h"

#include <stddef.h>
#include <netdb.h>

class WiFiManager;
class StatsManager;
class NMEA;
class AISContacts;

class NMEAWiFiSource : public TaskObject, WiFiManagerClient, public NMEASockSource {
    private:
        const char *ipv4Addr;
        uint16_t tcpPort;
        struct sockaddr_in sourceAddr;
        bool sourceAddrValid;

        DataModelBoolLeaf stateLeaf;

        virtual void task() override;

        static constexpr size_t stackSize = (1024 * 8);
        static constexpr uint32_t reconnectDelayMs = 1000;

    public:
        NMEAWiFiSource(const char *name, WiFiManager &wifiManager, StatsManager &statsManager,
                       const char *ipv4Addr, uint16_t tcpPort, NMEA &nmea,
                       AISContacts &aisContacts);
};

#endif
