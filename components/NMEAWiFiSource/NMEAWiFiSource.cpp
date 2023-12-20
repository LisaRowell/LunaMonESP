/*
 * This file is part of LunaMon (https://github.com/LisaRowell/LunaMonESP)
 * Copyright (C) 2021-2023 Lisa Rowell
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

#include "NMEAWiFiSource.h"

#include "TaskObject.h"
#include "WiFiManager.h"
#include "WiFiManagerClient.h"
#include "Logger.h"

#include "freertos/FreeRTOSConfig.h"
#include "freertos/task.h"

#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>

NMEAWiFiSource::NMEAWiFiSource(WiFiManager &wifiManager, const char *ipv4Addr, uint16_t tcpPort)
    : TaskObject("NMEAWiFiSource", LOGGER_LEVEL_DEBUG, stackSize),
      WiFiManagerClient(wifiManager),
      ipv4Addr(ipv4Addr), tcpPort(tcpPort) {
    if (inet_pton(AF_INET, ipv4Addr, &sourceAddr.sin_addr) == 1) {
        sourceAddrValid = true;
        sourceAddr.sin_family = AF_INET;
        sourceAddr.sin_port = htons(tcpPort);
    } else {
        sourceAddrValid = false;
        logger << logErrorNMEAWiFi << "Bad NMEA WiFi source IPv4 address '"
               << ipv4Addr << "'" << eol;
    }
}

void NMEAWiFiSource::task() {
    if (!sourceAddrValid) {
        logger << logNotifyNMEAWiFi << "Exiting WiFi NMEA source due to invalid address" << eol;
        return;
    }

    while (1) {
        if (!wifiConnected()) {
            logger << logDebugNMEAWiFi << "Waiting for WiFi to connect" << eol;
            waitForWiFiConnect();
        }

        int sock =  socket(AF_INET, SOCK_STREAM, IPPROTO_IP);
        if (sock == -1) {
            logger << logErrorNMEAWiFi << "Failed to create NMEA WiFi source socket" << eol;
            return;
        }

        logger << logDebugNMEAWiFi << "Starting connection to NMEA source " << ipv4Addr << ":"
               << tcpPort << eol;

        if (connect(sock, (struct sockaddr *)&sourceAddr, sizeof(sourceAddr)) != 0) {
            logger << logNotifyNMEAWiFi << "Failed to connect to WiFi NMEA source " << ipv4Addr
                   << ":" << tcpPort << ": " << strerror(errno) << eol;
        } else {
            logger << logDebugNMEAWiFi << "Connected to NMEA source "<< ipv4Addr << ":" << tcpPort
                   << eol;
            processNMEAStream(sock);
            close(sock);
        }

        vTaskDelay(pdMS_TO_TICKS(reconnectDelayMs));
    }
}
