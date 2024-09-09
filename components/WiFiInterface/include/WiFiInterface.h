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

#ifndef WIFI_INTERFACE_H
#define WIFI_INTERFACE_H

#include "Interface.h"
#include "InterfaceProtocol.h"
#include "WiFiManagerClient.h"
#include "DataModelBoolLeaf.h"

#include <sys/socket.h>
#include <stddef.h>
#include <stdint.h>

class StatsManager;
class DataModelNode;
class WiFiManager;

class WiFiInterface : public Interface, WiFiManagerClient {
    private:
        static constexpr uint32_t reconnectDelayMs = 1000;

        const char *ipv4Addr;
        uint16_t tcpPort;
        struct sockaddr_in sourceAddr;
        bool sourceAddrValid;
        int sock;

        DataModelBoolLeaf stateLeaf;

        void reconnectDelay();

    protected:
        bool interfaceConfigValid() const;
        bool connectToSource();
        size_t readToBuffer(void *buffer, size_t rxBufferSize);
        void sourceDisconnected();

    public:
        WiFiInterface(const char *name, const char *label, enum InterfaceProtocol protocol,
                      const char *ipv4Addr, uint16_t tcpPort, WiFiManager &wifiManager,
                      StatsManager &statsManager, DataModel &dataModel, size_t stackSize);
        virtual size_t sendBytes(const void *bytes, size_t length) override;
};

#endif // WIFI_INTERFACE_H
