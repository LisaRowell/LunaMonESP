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

#include "NMEAWiFiInterface.h"

#include "WiFiInterface.h"
#include "InterfaceProtocol.h"
#include "NMEA.h"
#include "NMEASource.h"

NMEAWiFiInterface::NMEAWiFiInterface(const char *name, const char *ipv4Addr, uint16_t tcpPort,
                                     WiFiManager &wifiManager, StatsManager &statsManager,
                                     NMEA &nmea, AISContacts &aisContacts)
    : WiFiInterface(name, INTERFACE_NMEA_O183, nmea.nmeaNode(), ipv4Addr, tcpPort,wifiManager,
                    stackSize),
      NMEASource(name, interfaceNode(), aisContacts, statsManager) {
}

void NMEAWiFiInterface::task() {
    if (!interfaceConfigValid() ) {
        logger << logNotifyNMEAWiFi << "Exiting WiFi NMEA interface due to invalid address" << eol;
        return;
    }

    while (1) {
        logger << logDebugNMEAWiFi << "Starting connection to NMEA WiFi source..." << eol;

        if (!connectToSource()) {
            logger << logErrorNMEAWiFi << "Fatal error connecting to WiFi NMEA source" << eol;
            return;
        }

        logger << logDebugNMEAWiFi << "Connected to NMEA WiFi source, starting read" << eol;
        processStream();

        logger << logDebugNMEAWiFi << "Disconnected from NMEA WiFi source" << eol;
        sourceDisconnected();
    }
}

void NMEAWiFiInterface::processStream() {
    sourceReset();

    while (true) {
        char buffer[maxNMEALineLength];
        size_t bytesRead = readToBuffer(buffer, maxNMEALineLength);
        if (bytesRead == 0) {
            // Connection was closed
            return;
        }
        processBuffer(buffer, bytesRead);
    }
}
