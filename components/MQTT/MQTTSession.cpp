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

#include "MQTTSession.h"

#include "MQTTBroker.h"
#include "MQTTConnectMessage.h"
#include "MQTTConnectAckMessage.h"

#include "Logger.h"
#include "Error.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <stdint.h>

MQTTSession::MQTTSession(MQTTBroker &broker, uint8_t id)
    : TaskObject("MQTTSession", LOGGER_LEVEL_DEBUG, stackSize),
      id(id), broker(broker), _connection(nullptr), freshSession(true) {
}

void MQTTSession::task() {
    while (true != false) {
        uint32_t notifications;
        (void)xTaskNotifyWaitIndexed(notifyIndex, 0, ULONG_MAX, &notifications, portMAX_DELAY);

        if (notifications & notifyShutdownMask) {
            shutdown();
        }

        if (notifications & notifyNewConnectionIdMask) {
            const unsigned newConnectionId =
                (notifications & notifyNewConnectionIdMask) >> notifyNewConnectionIdShift;
            newConnection(newConnectionId);
        }

        if (notifications & notifyMessageReadyMask) {
            readMessages();
        }
    }
}

// This method is called from the from a MQTTConnection thread with the session lock taken.
void MQTTSession::assignConnection(unsigned connectionId) {
    // We clear the notification bits used to convey new connections ids. If we actually for some
    // reason had a connection that hadn't been noticed by the session, we'll pick it up in the
    // return value.
    cancelPendingConnectionAssignment();

     if (xTaskNotifyIndexed(taskHandle(), notifyIndex, connectionId << notifyNewConnectionIdShift,
                            eSetBits) != pdPASS) {
        logger << logErrorMQTT << "Failed to send new connection notification to session #" << id
               << eol;
        errorExit();
    }
}

// This method is called from the from a MQTTConnection thread with the session lock taken.
void MQTTSession::initiateShutdown() {
    // It's possible that the session hadn't yet picked up the client so we first clear any pending
    // new client notification and if there was one, mark the Connection for disconnection.
    cancelPendingConnectionAssignment();

     if (xTaskNotifyIndexed(taskHandle(), notifyIndex, notifyShutdownMask, eSetBits) != pdPASS) {
        taskLogger() << logErrorMQTT << "Failed to send shutdown notification to session #" << id
                     << eol;
        errorExit();
    }
}

void MQTTSession::cancelPendingConnectionAssignment() {
    const uint32_t oldNotifications = ulTaskNotifyValueClear(taskHandle(),
                                                             notifyNewConnectionIdMask);
    const unsigned skippedConnectionId =
        (oldNotifications & notifyNewConnectionIdMask) >> notifyNewConnectionIdShift;
    if (skippedConnectionId) {
        MQTTConnection *skippedConnection = broker.connectionForId(skippedConnectionId);
        taskLogger() << logDebugMQTT << "Canceling skipped Connection (#" << skippedConnectionId
                     << ") assigned to session #" << id << eol;
        skippedConnection->markForDisconnection();
    }
}

bool MQTTSession::isForClient(const etl::istring &clientID) const {
    return this->clientID == clientID;
}

MQTTConnection *MQTTSession::connection() const {
    return _connection;
}

void MQTTSession::newConnection(unsigned connectionId) {
    // We might have been assigned a new connection while we still had an active connection. This
    // could happen if the client detected a disconnect faster than we did or if two clients
    // were trying to use the same ClientId, which would be a configuration issue on their end.
    if (_connection) {
        _connection->markForDisconnection();
    }

    _connection = broker.connectionForId(connectionId);
    connectionSocket = _connection->socket();
    clientID = _connection->clientID();
    connectionMessageBuffer = _connection->sessionMessageBuffer();

    logger << logDebugMQTT << "Session #" << id << " for " << clientID
           << " paired with connection #" << connectionId << eol;
}

void MQTTSession::readMessages() {
    size_t messageSize;
    uint8_t messageBuffer[maxIncomingMessageSize];

    while ((messageSize = xMessageBufferReceive(connectionMessageBuffer, messageBuffer,
                                                maxIncomingMessageSize, 0)) > 0) {
        MQTTMessage message = MQTTMessage(messageBuffer, messageSize);

        MQTTMessageType msgType = message.messageType();
        switch (msgType) {
            case MQTT_MSG_CONNECT:
                connectMessageReceived(message);
                break;

            default:
                logger << logDebugMQTT << "Session #" << id << " ignored a "
                       << message.messageTypeStr() << " message from client " << clientID << eol;
        }
    }
}

void MQTTSession::connectMessageReceived(MQTTMessage &message) {
    MQTTConnectMessage connectMessage(message);
    (void)connectMessage.parse();
    cleanSession = connectMessage.cleanSession();

    logger << "Session #" << id << " sending a CONNACK Accepted to " << clientID << eol;

    bool result = sendMQTTConnectAckMessage(connectionSocket, !freshSession, MQTT_CONNACK_ACCEPTED);
    // TODO:: Need to handle when the send fails as this could indicate the TCP connection closing
}

void MQTTSession::shutdown() {
    logger << logNotifyMQTT << "Shutting down session (#" << id << ") for client id " << clientID
           << eol;

    // If we have a connection currently, make sure we signal it to close and for sanity, clear our
    // references as well.
    if (_connection) {
        _connection->markForDisconnection();
        _connection = nullptr;
        connectionSocket = 0;
        clientID.clear();
    }

    broker.sessionGoingIdle(*this);
}

// Called from a Connection thread
void MQTTSession::notifyMessageReady() {
    if (xTaskNotifyIndexed(taskHandle(), notifyIndex, notifyMessageReadyMask, eSetBits) != pdPASS) {
        taskLogger() << logErrorMQTT << "Failed to send message ready notification to session #"
                     << id << eol;
        errorExit();
    }
}
