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

#ifndef NMEA_SERVER_H
#define NMEA_SERVER_H

#include "NMEALineHandler.h"
#include "TaskObject.h"
#include "WiFiManagerClient.h"
#include "StatsHolder.h"
#include "NMEAClient.h"

#include "StatCounter.h"
#include "DataModelNode.h"
#include "DataModelUInt8Leaf.h"
#include "DataModelUInt32Leaf.h"
#include "DataModelStringLeaf.h"

#include "etl/pool.h"
#include "etl/string.h"
#include "etl/vector.h"

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

#include <arpa/inet.h>
#include <lwip/sockets.h>

#include <stddef.h>
#include <stdint.h>

class WiFiManager;
class NMEALine;
class StatsManager;
class DataModel;

class NMEAServer : public NMEALineHandler, public TaskObject, WiFiManagerClient, StatsHolder {
    private:
        static constexpr size_t stackSize = 8 * 1024;
        static constexpr uint32_t lockTimeoutMs = 60 * 1000;
        static constexpr size_t maxClientNameLength = 22;

        uint16_t knownPort;
        int serverSocket;
        SemaphoreHandle_t clientLock;
        etl::pool<NMEAClient, CONFIG_LUNAMON_NMEA_SERVER_MAX_CLIENTS> clientPool;
        etl::vector<NMEAClient *, CONFIG_LUNAMON_NMEA_SERVER_MAX_CLIENTS> clients;

        uint32_t connects;
        uint32_t disconnects;
        uint32_t connectFailures;
        uint8_t maxClients;
        StatCounter sentMessages;
        uint32_t droppedMessages;
        DataModelNode nmeaServerNode;
        DataModelUInt32Leaf connectsLeaf;
        DataModelUInt32Leaf disconnectsLeaf;
        DataModelUInt32Leaf connectFailuresLeaf;
        DataModelNode clientsNode;
        DataModelUInt8Leaf activeClientsLeaf;
        DataModelUInt8Leaf maxClientsLeaf;
        // At some point this should be changed to match the configured max clients instead of
        // limiting to 5.
        etl::string<maxClientNameLength> client1NameBuffer;
        DataModelStringLeaf client1NameLeaf;
        etl::string<maxClientNameLength> client2NameBuffer;
        DataModelStringLeaf client2NameLeaf;
        etl::string<maxClientNameLength> client3NameBuffer;
        DataModelStringLeaf client3NameLeaf;
        etl::string<maxClientNameLength> client4NameBuffer;
        DataModelStringLeaf client4NameLeaf;
        etl::string<maxClientNameLength> client5NameBuffer;
        DataModelStringLeaf client5NameLeaf;
        DataModelStringLeaf *clientNameLeaves[5];
        DataModelNode messagesNode;
        DataModelUInt32Leaf sentMessagesLeaf;
        DataModelUInt32Leaf sendMessageRateLeaf;
        DataModelUInt32Leaf droppedMessagesLeaf;

        virtual void task() override;
        void createServerSocket();
        void newClient(int clientSocket, struct sockaddr_in &sourceAddr,
                       socklen_t sourceAddrLength);
        void closeClientSocket(int clientSocket);
        void takeClientLock();
        void releaseClientLock();
        virtual void exportStats(uint32_t msElapsed) override;
        void updateClientNames();

    public:
        NMEAServer(uint16_t knownPort, WiFiManager &wifiManager, StatsManager &statsManager,
                   DataModel &dataModel);
        virtual void handleLine(const NMEALine &inputLine, const NMEATalker &talker,
                                const NMEAMsgType &msgType) override;
};

#endif // NMEA_SERVER_H
