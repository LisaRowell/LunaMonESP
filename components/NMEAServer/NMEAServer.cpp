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

#include "NMEAServer.h"
#include "NMEAClient.h"

#include "NMEALine.h"

#include "WiFiManager.h"
#include "WiFiManagerClient.h"
#include "StatsManager.h"
#include "DataModel.h"
#include "DataModelNode.h"
#include "DataModelUInt8Leaf.h"

#include "Logger.h"
#include "Error.h"

#include "etl/algorithm.h"
#include "etl/pool.h"
#include "etl/vector.h"

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

#include <arpa/inet.h>
#include <lwip/sockets.h>

#include <stdint.h>

NMEAServer::NMEAServer(uint16_t knownPort, WiFiManager &wifiManager, StatsManager &statsManager,
                       DataModel &dataModel)
    : TaskObject("NMEA Server", LOGGER_LEVEL_DEBUG, stackSize),
      WiFiManagerClient(wifiManager),
      knownPort(knownPort),
      connects(0),
      disconnects(0),
      connectFailures(0),
      maxClients(0),
      sentMessages(),
      droppedMessages(0),
      nmeaServerNode("nmeaServer", &dataModel.sysNode()),
      connectsLeaf("connects", &nmeaServerNode),
      disconnectsLeaf("disconnects", &nmeaServerNode),
      connectFailuresLeaf("connectFailures", &nmeaServerNode),
      clientsNode("clients", &nmeaServerNode),
      activeClientsLeaf("active", &clientsNode),
      maxClientsLeaf("max", &clientsNode),
      client1NameLeaf("1", &clientsNode, client1NameBuffer),
      client2NameLeaf("2", &clientsNode, client2NameBuffer),
      client3NameLeaf("3", &clientsNode, client3NameBuffer),
      client4NameLeaf("4", &clientsNode, client4NameBuffer),
      client5NameLeaf("5", &clientsNode, client5NameBuffer),
      clientNameLeaves { &client1NameLeaf, &client2NameLeaf, &client3NameLeaf, &client4NameLeaf,
                         &client5NameLeaf },
      messagesNode("messages", &nmeaServerNode),
      sentMessagesLeaf("sent", &messagesNode),
      sendMessageRateLeaf("sendRate", &messagesNode),
      droppedMessagesLeaf("dropped", &messagesNode) {
    statsManager.addStatsHolder(*this);

    if ((clientLock = xSemaphoreCreateMutex()) == nullptr) {
        logger << logErrorNMEAServer << "Failed to create clientLock mutex" << eol;
        errorExit();
    }
}

void NMEAServer::task() {
    // We probably don't strictly need to do this, but there's little point in opening up the
    // server for business before WiFi is even connected.
    if (!wifiConnected()) {
        logger << logDebugNMEAServer << "Waiting for WiFi to connect" << eol;
        waitForWiFiConnect();
    }

    createServerSocket();
    logger << logNotifyNMEAServer << "NEMA Server listening for connections on port " << knownPort
           << eol;

    while (1) {
        struct sockaddr_in sourceAddr;
        socklen_t sourceAddrLength = sizeof(sourceAddr);
        int clientSocket = accept(serverSocket, (struct sockaddr *)&sourceAddr, &sourceAddrLength);
        if (clientSocket < 0) {
            logger << logWarnNMEAServer << "Unable to accept NMEA client connection: "
                   << strerror(errno) << eol;
            connectFailures++;
            continue;
        }

        newClient(clientSocket, sourceAddr, sourceAddrLength);
    }
}

void NMEAServer::createServerSocket() {
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) < 0) {
        logger << logErrorNMEAServer << "Failed to create NMEA Server socket: " << strerror(errno)
               << eol;
        errorExit();
    }

    int reuseAddrOption = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &reuseAddrOption, sizeof(reuseAddrOption));

    logger << logErrorNMEAServer << "NMEA Server socket created" << eol;

    struct sockaddr_in serverAddr;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(knownPort);

    if (bind(serverSocket,  (struct sockaddr *)&serverAddr, sizeof(serverAddr)) != 0) {
        logger << logErrorNMEAServer << "NMEA Server socket bind failed: " << strerror(errno)
               << eol;
        errorExit();
    }

    logger << logErrorNMEAServer << "NMEA Server socket bound to port " << knownPort << eol;

    if (listen(serverSocket, 1) != 0) {
        logger << logErrorNMEAServer << "Listen failed on NMEA Server socket: " << strerror(errno)
               << eol;
        errorExit();
    }
}

void NMEAServer::newClient(int clientSocket, struct sockaddr_in &sourceAddr,
                           socklen_t sourceAddrLength) {
    // We currently only support IPv4, and should not have any connections from an IPv6 address.
    if (sourceAddrLength != sizeof(sourceAddr)) {
        logger << logWarnNMEAServer << "We somehow got an IPv6 client. Closing it." << eol;
        closeClientSocket(clientSocket);
        connectFailures++;
        return;
    }

    takeClientLock();

    if (clientPool.full()) {
        releaseClientLock();
        logger << logWarnNMEAServer << "Maximum NMEA Server connections exceeded. "
               << "Rejecting connection from " << sourceAddr << eol;
        closeClientSocket(clientSocket);
        connectFailures++;
        return;
    }

    NMEAClient *client = new (clientPool.allocate()) NMEAClient(clientSocket, sourceAddr);
    if (client == nullptr) {
        // This shouldn't be possible...
        fatalError("NMEA Server client pool allocation failed despite being non-full");
    }

    clients.push_back(client);
    maxClients = etl::max(maxClients, (uint8_t)clients.size());

    releaseClientLock();

    logger << logNotifyNMEAServer << "Accepted connection from " << sourceAddr << eol;
    connects++;
}

// Called on the thread for the particular interface...
void NMEAServer::handleLine(const NMEALine &inputLine) {
    takeClientLock();

    etl::ivector<NMEAClient *>::iterator clientInterator;
    for (clientInterator = clients.begin(); clientInterator != clients.end(); ) {
        NMEAClient *client = *clientInterator;
        bool dropped;
        if (!client->sendLine(inputLine, dropped)) {
            // Send failed, shut it down...
            // Consider the implications to logging while holding this lock. It's not great, but
            // the information is pretty useful.
            logger << logNotifyNMEAServer << "NMEA Server client " << client << " closed." << eol;
            clientInterator = clients.erase(clientInterator);
            client->~NMEAClient();
            clientPool.release(client);
            disconnects++;
        } else {
            if (dropped) {
                droppedMessages++;
            } else {
                sentMessages++;
            }
            clientInterator++;
        }
    }

    releaseClientLock();
}

void NMEAServer::closeClientSocket(int clientSocket) {
    shutdown(clientSocket, SHUT_RDWR);
    close(clientSocket);
}

void NMEAServer::takeClientLock() {
    if (xSemaphoreTake(clientLock, pdMS_TO_TICKS(lockTimeoutMs)) != pdTRUE) {
        taskLogger() << logErrorNMEAServer << "Failed to get client lock mutex" << eol;
        errorExit();
    }
}

void NMEAServer::releaseClientLock() {
    xSemaphoreGive(clientLock);
}

void NMEAServer::exportStats(uint32_t msElapsed) {
    activeClientsLeaf = clients.size();
    connectsLeaf = connects;
    disconnectsLeaf = disconnects;
    connectFailuresLeaf = connectFailures;
    maxClientsLeaf = maxClients;
    sentMessages.update(sentMessagesLeaf, sendMessageRateLeaf, msElapsed);
    droppedMessagesLeaf = droppedMessages;

    updateClientNames();
}

void NMEAServer::updateClientNames() {
    takeClientLock();

    int pos = 0;
    for (NMEAClient *client : clients) {
        if (pos < 5) {
            // For now we just have a fixed number of name slots in the data model.
            client->setNameLeaf(clientNameLeaves[pos]);
            pos++;
        } else {
            break;
        }
    }

    releaseClientLock();

    for ( ; pos < 5; pos++) {
        *clientNameLeaves[pos] = "";
    }
}
