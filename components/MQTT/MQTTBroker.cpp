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

#include "MQTTBroker.h"
#include "MQTTConnection.h"
#include "MQTTSession.h"

#include "Logger.h"
#include "Error.h"
#include "ESPError.h"

#include "etl/intrusive_links.h"
#include "etl/intrusive_list.h"

#include <lwip/sockets.h>

#include <freertos/semphr.h>

MQTTBroker::MQTTBroker(WiFiManager &wifiManager)
    : TaskObject("NMEAWiFiSource", LOGGER_LEVEL_DEBUG, stackSize),
      WiFiManagerClient(wifiManager) {
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
        MQTTSession *session = new MQTTSession(*this, sessionId);
        if (!session) {
            logger << logErrorMQTT << "Failed to allocation session " << sessionId << eol;
            errorExit();
        }
        freeSessions.push_back(*session);
        session->start();
    }
    releaseSessionLock();
}

MQTTConnection *MQTTBroker::connectionForId(uint8_t connectionId) const {
    return connectionIndex[connectionId];
}

void MQTTBroker::task() {
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

    logger << logNotifyMQTT << "Accepted connection from " << sourceAddr << eol;
    connection.assignSocket(connectionSocket, sourceAddr);
}

void MQTTBroker::connectionGoingIdle(MQTTConnection &connection) {
    takeConnectionLock();

    // Remove from the active connection list then append to the idle list
    etl::unlink<ConnectionLink>(connection);
    idleConnections.push_back(connection);

    releaseConnectionLock();
}
        bool pairConnectionWithCleanSession(MQTTConnection *connection);

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

void MQTTBroker::closeConnectionSocket(int connectionSocket) {
    shutdown(connectionSocket, SHUT_RDWR);
    close(connectionSocket);
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
