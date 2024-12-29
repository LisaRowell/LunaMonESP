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

#include "MQTTBroker.h"
#include "MQTTConnection.h"
#include "MQTTSession.h"

#include "DataModel.h"

#include "StatsManager.h"

#include "Logger.h"

#include "Error.h"
#include "ESPError.h"

#include "etl/intrusive_links.h"
#include "etl/intrusive_list.h"

#include <lwip/sockets.h>

#include <freertos/semphr.h>

#include <stdint.h>

MQTTBroker::MQTTBroker(WiFiManager &wifiManager, DataModel &dataModel, StatsManager &statsManager)
    : TaskObject("MQTTBroker", LOGGER_LEVEL_DEBUG, stackSize),
      WiFiManagerClient(wifiManager),
      clientsNode("clients", &dataModel.brokerNode()),
      connectedClientsLeaf("connected", &clientsNode),
      disconnectedClientsLeaf("disconnected", &clientsNode),
      maximumClientsLeaf("maximum", &clientsNode),
      totalClientsLeaf("total", &clientsNode),
      messagesReceivedLeaf("received", &dataModel.messagesNode()),
      messagesSentLeaf("sent", &dataModel.messagesNode()),
      publishNode("publish", &dataModel.messagesNode()),
      publishReceivedLeaf("received", &publishNode),
      publishSentLeaf("sent", &publishNode),
      publishDroppedLeaf("dropped", &publishNode),
      connectionsNode("connections", &dataModel.brokerNode()),
      connection1IDLeaf("1", &connectionsNode, connection1IDBuffer),
      connection2IDLeaf("2", &connectionsNode, connection2IDBuffer),
      connection3IDLeaf("3", &connectionsNode, connection3IDBuffer),
      connection4IDLeaf("4", &connectionsNode, connection4IDBuffer),
      connection5IDLeaf("5", &connectionsNode, connection5IDBuffer),
      connectionLeaves { &connection1IDLeaf, &connection2IDLeaf, &connection3IDLeaf,
                         &connection4IDLeaf, &connection5IDLeaf },
      sessionsNode("sessions", &dataModel.brokerNode()),
      session1IDLeaf("1", &sessionsNode, session1IDBuffer),
      session2IDLeaf("2", &sessionsNode, session2IDBuffer),
      session3IDLeaf("3", &sessionsNode, session3IDBuffer),
      session4IDLeaf("4", &sessionsNode, session4IDBuffer),
      session5IDLeaf("5", &sessionsNode, session5IDBuffer),
      sessionLeaves { &session1IDLeaf, &session2IDLeaf, &session3IDLeaf, &session4IDLeaf,
                      &session5IDLeaf } {
    if ((connectionLock = xSemaphoreCreateMutex()) == nullptr) {
        logger << logErrorMQTT << "Failed to create connectionLock mutex" << eol;
        errorExit();
    }

    if ((sessionLock = xSemaphoreCreateMutex()) == nullptr) {
        logger << logErrorMQTT << "Failed to create sessionLock mutex" << eol;
        errorExit();
    }

    // We preallocate the maximum number connections to avoid allocating and freeing them as clients
    // come and go. Each connection has a task, which we fire up now, putting it in a wait state for
    // when we later assign a socket to it.
    takeConnectionLock();
    for (unsigned connectionId = 1; connectionId <= maxMQTTConnections; connectionId++) {
        MQTTConnection *connection = new MQTTConnection(*this, connectionId);
        if (!connection) {
            logger << logErrorMQTT << "Failed to allocation connection " << connectionId << eol;
            errorExit();
        }

        connectionIndex[connectionId] = connection;
        idleConnections.push_back(*connection);
        connection->start();
    }
    releaseConnectionLock();

    // We preallocate the maximum number connections to avoid allocating and freeing them as clients
    // come and go. Each connection has a task, which we fire up now, putting it in a wait state for
    // when we later assign a connection to it.
    takeSessionLock();
    for (unsigned sessionId = 1; sessionId <= maxMQTTSessions; sessionId++) {
        MQTTSession *session = new MQTTSession(*this, dataModel, sessionId);
        if (!session) {
            logger << logErrorMQTT << "Failed to allocation session " << sessionId << eol;
            errorExit();
        }
        freeSessions.push_back(*session);
        session->start();
    }
    releaseSessionLock();

    statsManager.addStatsHolder(*this);
}

MQTTConnection *MQTTBroker::connectionForId(uint8_t connectionId) const {
    return connectionIndex[connectionId];
}

void MQTTBroker::task() {
    initClientStats();

    // We probably don't strictly need to do this, but there's little point in opening up the
    // server for business before WiFi is even connected.
    if (!wifiConnected()) {
        logger << logDebugMQTT << "Waiting for WiFi to connect" << eol;
        waitForWiFiConnect();
    }

    createServerSocket();
    logger << logNotifyMQTT << "MQTT Broker listening for connections on port " << serverPort
           << eol;

    while (1) {
        struct sockaddr_in sourceAddr;
        socklen_t sourceAddrLength = sizeof(sourceAddr);
        int connectionSocket = accept(serverSocket, (struct sockaddr *)&sourceAddr,
                                      &sourceAddrLength);
        if (connectionSocket < 0) {
            logger << logWarnMQTT << "Unable to accept MQTT connection: " << strerror(errno) << eol;
            continue;
        }

        newConnection(connectionSocket, sourceAddr, sourceAddrLength);
    }
}

void MQTTBroker::createServerSocket() {
    if ((serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_IP)) < 0) {
        logger << logErrorMQTT << "Failed to create MQTT server socket: " << strerror(errno) << eol;
        errorExit();
    }

    int reuseAddrOption = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &reuseAddrOption, sizeof(reuseAddrOption));

    logger << logDebugMQTT << "MQTT server socket created" << eol;

    struct sockaddr_in serverAddr;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(serverPort);

    if (bind(serverSocket,  (struct sockaddr *)&serverAddr, sizeof(serverAddr)) != 0) {
        logger << logErrorMQTT << "MQTT server socket bind failed: " << strerror(errno) << eol;
        errorExit();
    }

    logger << logDebugMQTT << "MQTT server socket bound to port " << serverPort << eol;

    if (listen(serverSocket, 1) != 0) {
        logger << logErrorMQTT << "Listen failed on MQTT server socket: " << strerror(errno) << eol;
        errorExit();
    }
}

void MQTTBroker::newConnection(int connectionSocket, struct sockaddr_in &sourceAddr,
                               socklen_t sourceAddrLength) {
    // We currently only support IPv4, and should not have any connections from an IPv6 address.
    if (sourceAddrLength != sizeof(sourceAddr)) {
        logger << logWarnMQTT << "We somehow got an IPv6 connection. Closing it." << eol;
        closeConnectionSocket(connectionSocket);
        return;
    }

    takeConnectionLock();

    if (idleConnections.empty()) {
        releaseConnectionLock();
        logger << logWarnMQTT << "Max MQTT connections exceeding, ignoring connection from "
               << sourceAddr << eol;
        closeConnectionSocket(connectionSocket);
        return;
    }

    MQTTConnection &connection = idleConnections.front();
    idleConnections.pop_front();
    activeConnections.push_back(connection);

    releaseConnectionLock();

    logger << logNotifyMQTT << "Accepted MQTT connection from " << sourceAddr << eol;
    connection.assignSocket(connectionSocket, sourceAddr);
}

void MQTTBroker::connectionGoingIdle(MQTTConnection &connection) {
    takeConnectionLock();

    // Remove from the active connection list then append to the idle list
    etl::unlink<ConnectionLink>(connection);
    idleConnections.push_back(connection);

    releaseConnectionLock();
}

MQTTSession *MQTTBroker::pairConnectionWithSession(MQTTConnection *connection, bool cleanSession) {
    if (cleanSession) {
        return pairConnectionWithCleanSession(connection);
    } else {
        return pairConnectionWithNonCleanSession(connection);
    }
}

// Invoked by a new connection with the Clean Session flag not set to get a session. If there is an
// existing session, either disconnected or possibly connected, we need to signal for it to be
// shutdown and a free session should assigned to this connection.
MQTTSession *MQTTBroker::pairConnectionWithCleanSession(MQTTConnection *connection) {
    const etl::istring &clientID = connection->clientID();

    takeSessionLock();

    // First look for a disconnected session with the same clientID as this connection. If we find
    // one, mark it for shutdown.
    for (MQTTSession &session : disconnectedSessions) {
        if (session.isForClient(clientID)) {
            // Signal to the session to shut itself down. It will handle moving itself to the
            // free list.
            session.initiateShutdown();
            break;
        }
    }

    // Make sure there isn't an active session with the same clientID. This could happen if the
    // stack of the client realized the TCP connection failed before we did or if there were two
    // client instances using the same clientID.
    for (MQTTSession &session : activeSessions) {
        if (session.isForClient(clientID)) {
            // Signal to the session to shut itself down. It will handle closing the connection and
            // moving itself to the free list
            session.initiateShutdown();
            break;
        }
    }

    // While there might be a session closing down, we're free to start a fresh one up with a free
    // session.
    if (!freeSessions.empty()) {
        MQTTSession &session = freeSessions.front();
        freeSessions.pop_front();
        activeSessions.push_back(session);

        session.assignConnection(connection->id());
        releaseSessionLock();
        return &session;
    } else {
        releaseSessionLock();
        return nullptr;
    }
}

MQTTSession *MQTTBroker::pairConnectionWithNonCleanSession(MQTTConnection *connection) {
    const etl::istring &clientID = connection->clientID();

    takeSessionLock();

    // First look for a disconnected session with the same clientID as this connection.
    for (MQTTSession &session : disconnectedSessions) {
        if (session.isForClient(clientID)) {
            etl::unlink<SessionLink>(session);
            activeSessions.push_back(session);

            session.assignConnection(connection->id());
            releaseSessionLock();
            return &session;
        }
    }

    // Make sure we don't have an active session for the same clientID. If we do, give it the new
    // connection id and let it handle closing the existing connection.
    for (MQTTSession &session : activeSessions) {
        if (session.isForClient(clientID)) {
            // While the session make actually still have a connection, we let it handle closing it
            session.assignConnection(connection->id());
            releaseSessionLock();
            return &session;
        }
    }

    // There isn't a session for this connection currently, so try to get one from the free list.
    if (!freeSessions.empty()) {
        MQTTSession &session = freeSessions.front();
        freeSessions.pop_front();
        activeSessions.push_back(session);

        session.assignConnection(connection->id());
        releaseSessionLock();
        return &session;
    } else {
        releaseSessionLock();
        return nullptr;
    }
}

void MQTTBroker::sessionGoingIdle(MQTTSession &session) {
    takeSessionLock();

    // Remove which ever list the session is on (active or disconnected) then append to the free
    // list
    etl::unlink<SessionLink>(session);
    freeSessions.push_back(session);

    releaseSessionLock();
}

void MQTTBroker::sessionLostConnection(MQTTSession &session) {
    takeSessionLock();

    // Remove which ever list the session is on (active or disconnected) then append to the
    // disconnected list
    etl::unlink<SessionLink>(session);
    disconnectedSessions.push_back(session);

    releaseSessionLock();
}

void MQTTBroker::closeConnectionSocket(int connectionSocket) {
    shutdown(connectionSocket, SHUT_RDWR);
    close(connectionSocket);
}

void MQTTBroker::initClientStats() {
    connectedClientsLeaf = 0;
    disconnectedClientsLeaf = 0;
    maximumClientsLeaf = 0;
    totalClientsLeaf = 0;
}

void MQTTBroker::exportStats(uint32_t msElapsed) {
    exportMessageStats();
    exportConnectionInfo();
    exportSessionInfo();
}

void MQTTBroker::exportMessageStats() {
    uint32_t received = 0;
    uint32_t sent = 0;
    uint32_t publishReceived = 0;
    uint32_t publishSent = 0;
    uint32_t publishDropped = 0;

    // We loop through both active and inactive connections and sessions when we gather sent message
    // counts sine what we want is the accumulation since broker startup and not stats about active
    // sessions.
    takeConnectionLock();
    for (MQTTConnection &activeConnection : activeConnections) {
        sent += activeConnection.messagesSent();
    }
    for (MQTTConnection &idleConnection : idleConnections) {
        sent += idleConnection.messagesSent();
    }
    releaseConnectionLock();

    takeSessionLock();
    for (MQTTSession &activeSession : activeSessions) {
        received += activeSession.messagesReceived();
        sent += activeSession.messagesSent();
        publishReceived += activeSession.publishMessagesReceived();
        publishSent += activeSession.publishMessagesSent();
        publishDropped += activeSession.publishMessagesDropped();
    }
    for (MQTTSession &disconnectedSession : disconnectedSessions) {
        received += disconnectedSession.messagesReceived();
        sent += disconnectedSession.messagesSent();
        publishReceived += disconnectedSession.publishMessagesReceived();
        publishSent += disconnectedSession.publishMessagesSent();
        publishDropped += disconnectedSession.publishMessagesDropped();
    }
    for (MQTTSession &freeSession : freeSessions) {
        received += freeSession.messagesReceived();
        sent += freeSession.messagesSent();
        publishReceived += freeSession.publishMessagesReceived();
        publishSent += freeSession.publishMessagesSent();
        publishDropped += freeSession.publishMessagesDropped();
    }
    releaseSessionLock();

    messagesReceivedLeaf = received;
    messagesSentLeaf = sent;
    publishReceivedLeaf = publishReceived;
    publishSentLeaf = publishSent;
    publishDroppedLeaf = publishDropped;
}

void MQTTBroker::exportConnectionInfo() {
    takeConnectionLock();

    unsigned connectionPos = 0;
    for (MQTTConnection &connection : activeConnections) {
        *connectionLeaves[connectionPos++] = connection.clientID();
    }

    for (; connectionPos < maxMQTTConnections; connectionPos++) {
        *connectionLeaves[connectionPos] = "";
    }

    releaseConnectionLock();
}

void MQTTBroker::exportSessionInfo() {
    uint8_t connectedClients = 0;
    uint8_t disconnectedClients = 0;

    takeSessionLock();

    unsigned sessionPos = 0;
    for (MQTTSession &activeSession : activeSessions) {
        connectedClients++;
        *sessionLeaves[sessionPos++] = activeSession.getClientID();
    }

    for (MQTTSession &disconnectedSession : disconnectedSessions) {
        disconnectedClients++;
        *sessionLeaves[sessionPos++] = disconnectedSession.getClientID();
    }

    releaseSessionLock();

    for (; sessionPos < maxMQTTSessions; sessionPos++) {
        *sessionLeaves[sessionPos] = "";
    }

    uint8_t totalClientsCount = connectedClients + disconnectedClients;
    totalClientsLeaf = totalClientsCount;
    connectedClientsLeaf = connectedClients;
    disconnectedClientsLeaf = disconnectedClients;
    if (totalClientsCount > maximumClientsLeaf) {
        maximumClientsLeaf = totalClientsCount;
    }
}

void MQTTBroker::takeConnectionLock() {
    if (xSemaphoreTake(connectionLock, pdMS_TO_TICKS(lockTimeoutMs)) != pdTRUE) {
        taskLogger() << logErrorMQTT << "Failed to get connection lock mutex" << eol;
        errorExit();
    }
}

void MQTTBroker::releaseConnectionLock() {
    xSemaphoreGive(connectionLock);
}

void MQTTBroker::takeSessionLock() {
    if (xSemaphoreTake(sessionLock, pdMS_TO_TICKS(lockTimeoutMs)) != pdTRUE) {
        taskLogger() << logErrorMQTT << "Failed to get session lock mutex" << eol;
        errorExit();
    }
}

void MQTTBroker::releaseSessionLock() {
    xSemaphoreGive(sessionLock);
}
