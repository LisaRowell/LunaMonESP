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

#include "WiFiInterface.h"
#include "Interface.h"
#include "InterfaceProtocol.h"
#include "WiFiManagerClient.h"

#include "Logger.h"

#include "freertos/FreeRTOSConfig.h"
#include "freertos/task.h"

#include <stddef.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

WiFiInterface::WiFiInterface(const char *name, enum InterfaceProtocol protocol,
                             const char *ipv4Addr, uint16_t tcpPort, WiFiManager &wifiManager,
                             DataModel &dataModel, size_t stackSize)
    : Interface(name, protocol, dataModel, stackSize),
      WiFiManagerClient(wifiManager),
      ipv4Addr(ipv4Addr),
      tcpPort(tcpPort),
      sock(0),
      stateLeaf("state", &interfaceNode()) {
    if (inet_pton(AF_INET, ipv4Addr, &sourceAddr.sin_addr) == 1) {
        sourceAddrValid = true;
        sourceAddr.sin_family = AF_INET;
        sourceAddr.sin_port = htons(tcpPort);
    } else {
        sourceAddrValid = false;
        logger << logErrorWiFiInterface << "Bad WiFi source IPv4 address '" << ipv4Addr << "'"
               << eol;
    }
}

bool WiFiInterface::interfaceConfigValid() const {
    return sourceAddrValid;
}

bool WiFiInterface::connectToSource() {
    while (1) {
        if (!wifiConnected()) {
            logger << logDebugWiFiInterface << "Waiting for WiFi to connect" << eol;
            waitForWiFiConnect();
        }

        sock = socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
        if (sock == -1) {
            logger << logErrorWiFiInterface << "Failed to create WiFi source socket: "
                   << strerror(errno) << "(" << errno << ")" << eol;
            return false;
        }

        logger << logDebugWiFiInterface << "Starting connection to WiFi source " << ipv4Addr << ":"
               << tcpPort << eol;

        if (connect(sock, (struct sockaddr *)&sourceAddr, sizeof(sourceAddr)) != 0) {
            logger << logNotifyWiFiInterface << "Failed to connect to WiFi NMEA source " << ipv4Addr
                   << ":" << tcpPort << ": " << strerror(errno) << eol;
            close(sock);
        } else {
            logger << logDebugNMEAWiFi << "Connected to NMEA source "<< ipv4Addr << ":" << tcpPort
                   << eol;
            stateLeaf = true;
            return true;
        }

        // Connection failed, but not in a fatal way. Try again...
        reconnectDelay();
    }
}

size_t WiFiInterface::readToBuffer(void *buffer, size_t rxBufferSize) {
    ssize_t bytesRead = recv(sock, buffer, rxBufferSize, 0);
    if (bytesRead == 0) {
        logger << logWarnWiFiInterface << "WiFi source connection closed." << eol;
        return 0;
    } else if (bytesRead < 0) {
        logger << logWarnWiFiInterface << "WiFi source read failed:" << strerror(errno) << "("
               << errno << ")" << eol;
        return 0;
    } else {
        return (size_t)bytesRead;
    }
}

void WiFiInterface::sourceDisconnected() {
    stateLeaf = false;
    close(sock);

    reconnectDelay();
}

void WiFiInterface::reconnectDelay() {
    vTaskDelay(pdMS_TO_TICKS(reconnectDelayMs));
}
