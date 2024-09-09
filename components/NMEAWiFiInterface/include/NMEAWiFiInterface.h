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

#ifndef NMEA_WIFI_INTERFACE_H
#define NMEA_WIFI_INTERFACE_H

#include "WiFiInterface.h"
#include "NMEAInterface.h"
#include "InterfaceProtocol.h"

#include <stddef.h>

class WiFiManager;
class StatsManager;
class AISContacts;
class DataModel;

class NMEAWiFiInterface : public WiFiInterface, public NMEAInterface {
    private:
        static constexpr size_t stackSize = (1024 * 8);

        virtual void task() override;
        void processStream();

    public:
        NMEAWiFiInterface(const char *name, const char *label, const char *ipv4Addr,
                          uint16_t tcpPort, WiFiManager &wifiManager, StatsManager &statsManager,
                          AISContacts &aisContacts, DataModel &dataModel);
};

#endif // NMEA_WIFI_INTERFACE_H
