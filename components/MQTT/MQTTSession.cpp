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

#include "MQTTSession.h"

#include "MQTTBroker.h"
#include "MQTTConnectMessage.h"
#include "MQTTConnectAckMessage.h"
#include "MQTTSubscribeMessage.h"
#include "MQTTSubscribeAckMessage.h"
#include "MQTTUnsubscribeMessage.h"
#include "MQTTUnsubscribeAckMessage.h"
#include "MQTTPingRequestMessage.h"
#include "MQTTPingResponseMessage.h"
#include "MQTTPublishMessage.h"
#include "MQTTDisconnectMessage.h"
#include "MQTTString.h"

#include "DataModel.h"

#include "Logger.h"
#include "Error.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <stdint.h>

MQTTSession::MQTTSession(MQTTBroker &broker, DataModel &dataModel, uint8_t id)
    : TaskObject("MQTTSession", LOGGER_LEVEL_DEBUG, stackSize),
      id(id), broker(broker), dataModel(dataModel), _connection(nullptr), freshSession(true) {
}

void MQTTSession::task() {
    while (true != false) {
        uint32_t notifications;
        (void)xTaskNotifyWaitIndexed(notifyIndex, 0, ULONG_MAX, &notifications, portMAX_DELAY);

        if (notifications & notifyShutdownMask) {
            shutdown();
        }

        if (notifications & notifyConnectionLostMask) {
            // It's possible that we detected the connection closing while doing a write and
            // signaled the connection to shutdown, but the connection shutdown before getting the
            // signal and also signalled us to shut down. Hey Mo!
            // We ignore the signal if we're not connected, but this needs to be considered more
            // closely as there could be a race condition here. Would it be better to not signal the
            // session if it's not connected to the connection in question?
            if (_connection != nullptr) {
                connectionLost();
            }
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

void MQTTSession::connectionLost() {
    // The Connection has signaled us that it has detected that the TCP connection closed. We should
    // notify it that it should go idle, and if we're a clean session go idle ourselvses. If we
    // we're set up as a clean connection we should let the broker know that we're a disconnected
    // session.
    _connection->markForDisconnection();
    _connection = nullptr;

    if (cleanSession) {
        logger << logDebugMQTT << "Session #" << id << " lost connection to " << clientID
               << ". Going idle." << eol;

        clientID.clear();
        broker.sessionGoingIdle(*this);
    } else {
        logger << logDebugMQTT << "Session #" << id << " lost connection to " << clientID
               << ". Going into disconnected." << eol;
        broker.sessionLostConnection(*this);
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

    if (freshSession) {
        // Until we suck in a CONNECT message from the connection, we mark the session as being a
        // clean session in case we get disconnected before the message comes in (unlikely) and
        // reuse a session that shouldn't have been.
        cleanSession = true;

        logger << logDebugMQTT << "Session #" << id << " for " << clientID
               << " paired with connection #" << connectionId << eol;
    } else {
        logger << logDebugMQTT << "Session #" << id << " for " << clientID
               << " repaired with connection #" << connectionId << eol;
    }
}

void MQTTSession::readMessages() {
    size_t messageSize;
    uint8_t messageBuffer[maxIncomingMessageSize];

    while (_connection &&
           (messageSize = xMessageBufferReceive(connectionMessageBuffer, messageBuffer,
                                                maxIncomingMessageSize, 0)) > 0) {
        MQTTMessage message = MQTTMessage(messageBuffer, messageSize);

        MQTTMessageType msgType = message.messageType();
        switch (msgType) {
            case MQTT_MSG_CONNECT:
                connectMessageReceived(message);
                break;

            case MQTT_MSG_CONNACK:
            case MQTT_MSG_SUBACK:
            case MQTT_MSG_UNSUBACK:
            case MQTT_MSG_PINGRESP:
                serverOnlyMsgReceivedError(message);
                break;

            case MQTT_MSG_SUBSCRIBE:
                subscribeMessageReceived(message);
                break;

            case MQTT_MSG_UNSUBSCRIBE:
                unsubscribeMessageReceived(message);
                break;

            case MQTT_MSG_PINGREQ:
                pingRequestMessageReceived(message);
                break;

            case MQTT_MSG_DISCONNECT:
                disconnectMessageReceived(message);
                break;

            case MQTT_MSG_RESERVED1:
            case MQTT_MSG_RESERVED2:
                reservedMsgReceivedError(message);
                break;

            case MQTT_MSG_PUBREC:
                // publishMessagesReceived++;
                logger << logWarnMQTT << "Received unimplemented message type "
                       << message.messageTypeStr() << " from client " << clientID << eol;
                break;

            case MQTT_MSG_PUBACK:
            case MQTT_MSG_PUBREL:
            case MQTT_MSG_PUBCOMP:
            default:
                logger << logWarnMQTT << "Received unimplemented message type "
                       << message.messageTypeStr() << " from client " << clientID << eol;
        }
    }
}

void MQTTSession::connectMessageReceived(MQTTMessage &message) {
    MQTTConnectMessage connectMessage(message);
    if (!connectMessage.parse()) {
        // Since the Connection had to already parse the CONNECT message, this should not have
        // failed.
        logger << logErrorMQTT << "Session #" << id << "'s parsing of CONNECT message from "
               << clientID << " failed." << eol;
        errorExit();
    }
    cleanSession = connectMessage.cleanSession();

    logger << "Session #" << id << " sending a CONNACK Accepted to " << clientID << eol;

    if (!sendMQTTConnectAckMessage(connectionSocket, !freshSession, MQTT_CONNACK_ACCEPTED)) {
        logger << logWarnMQTT << "Failed to send CONNACK message to client " << clientID
               << ". Closing connection." << eol;
        handleConnectionSendFailure();
    }
}

void MQTTSession::subscribeMessageReceived(MQTTMessage &message) {
    MQTTSubscribeMessage subscribeMessage(message);
    if (!subscribeMessage.parse()) {
        logger << logWarnMQTT << "Bad subscribe message from client '" << clientID
               << "'. Terminating connection." << eol;
        shutdown();
        return;
    }

    resetKeepAliveTimer();

    // Loop through the topics, trying to subscribe to each and adding the result to the SUBACK
    // message.
    unsigned topicFilterCount = subscribeMessage.numTopicFilters();
    if (topicFilterCount > maxTopicsPerSubscribeMessage) {
        logger << logWarnMQTT << "MQTT SUBSCRIBE from client " << clientID
               << " has too many topic filters (" << topicFilterCount
               << "). Terminating connection." << eol;
        shutdown();
    }
    uint8_t subscribeResults[maxTopicsPerSubscribeMessage];

    unsigned topicFilterIndex;
    for (topicFilterIndex = 0; topicFilterIndex < topicFilterCount; topicFilterIndex++) {
        MQTTString *topicFilterStr;
        uint8_t maxQoS;
        if (!subscribeMessage.getTopicFilter(topicFilterStr, maxQoS)) {
            logger << logWarnMQTT << "MQTT SUBSCRIBE from client " << clientID
                   << " has a messed up number of topic filters. Terminating connection." << eol;
            shutdown();
            return;
        }

        logger << logDebugMQTT << "MQTT Client '" << clientID << "' wants to subscribe to '"
               << *topicFilterStr << "' with max QoS " << maxQoS << eol;

        char topicFilter[maxTopicNameLength + 1];
        if (!topicFilterStr->copyTo(topicFilter, maxTopicNameLength)) {
            logger << logWarnMQTT << "MQTT SUBSCRIBE message with too long of a Topic Filter '"
                   << *topicFilterStr << "'" << eol;
            subscribeResults[topicFilterIndex] = mqttSubscribeResult(false, 0);
        } else {
            if (dataModel.subscribe(topicFilter, *this, (uint32_t)maxQoS)) {
                logger << logDebugMQTT << "Topic Filter '" << topicFilter << "' subscribed to by '"
                       << clientID << "'" << eol;
                subscribeResults[topicFilterIndex] = mqttSubscribeResult(true, 0);
            } else {
                logger << logWarnMQTT << "Client '" << clientID
                       << "' failed to subscribe to Topic Filter '" << topicFilter << "'" << eol;
                subscribeResults[topicFilterIndex] = mqttSubscribeResult(false, 0);
           }
        }
    }

    logger << logDebugMQTT << "Sending SUBACK message with " << topicFilterCount
           << " results to Client '" << clientID << "'" << eol;
    if (!sendMQTTSubscribeAckMessage(connectionSocket, subscribeMessage.packetId(),
                                     topicFilterCount, subscribeResults)) {
        logger << logErrorMQTT << "Failed to send SUBACK message to Client '" << clientID << "'"
               << eol;
        handleConnectionSendFailure();
    }
}

void MQTTSession::unsubscribeMessageReceived(MQTTMessage &message) {
    MQTTUnsubscribeMessage unsubscribeMessage(message);
    if (!unsubscribeMessage.parse()) {
        logger << logWarnMQTT << "Bad unsubscribe message from client '" << clientID
               << "'. Terminating connection." << eol;
        shutdown();
        return;
    }

    resetKeepAliveTimer();

    unsigned topicFilterCount = unsubscribeMessage.numTopicFilters();
    unsigned topicFilterIndex;
    for (topicFilterIndex = 0; topicFilterIndex < topicFilterCount; topicFilterIndex++) {
        MQTTString *topicFilterStr;
        if (!unsubscribeMessage.getTopicFilter(topicFilterStr)) {
            logger << logWarnMQTT << "MQTT UNSUBSCRIBE from client " << clientID
                   << " has a messed up number of topic filters. Terminating connection." << eol;
            shutdown();
            return;
        }

        logger << logDebugMQTT << "MQTT Client '" << clientID
               << "' wants to unsubscribe from '" << *topicFilterStr << "'" << eol;

        char topicFilter[maxTopicNameLength + 1];
        if (!topicFilterStr->copyTo(topicFilter, maxTopicNameLength)) {
            logger << logWarnMQTT << "MQTT UNSUBSCRIBE message with too long of a Topic Filter '"
                   << *topicFilterStr << "'" << eol;
        } else {
            dataModel.unsubscribe(topicFilter, *this);
            logger << logDebugMQTT << "Topic Filter '" << topicFilter << "' unsubscribed from by '"
                   << clientID << "'" << eol;
        }
    }

    logger << logDebugMQTT << "Sending UNSUBACK message to client '" << clientID << "'"
           << eol;
    if (!sendMQTTUnsubscribeAckMessage(connectionSocket, unsubscribeMessage.packetId())) {
        logger << logErrorMQTT << "Failed to send UNSUBACK message to client '" << clientID
               << "'" << eol;
        handleConnectionSendFailure();
    }
}

void MQTTSession::pingRequestMessageReceived(MQTTMessage &message) {
    logger << logErrorMQTT << "Received ping request message from client " << clientID << "."
           << eol;

    MQTTPingRequestMessage pingRequestMessage(message);

    if (!pingRequestMessage.parse()) {
        logger << logErrorMQTT << "Bad MQTT PINGREQ message from clent " << clientID
               << ". Terminating connection." << eol;
        shutdown();
        return;
    }

    resetKeepAliveTimer();

    logger << logDebugMQTT << "Sending MQTT PINGRESP message to client '" << clientID << eol;

    if (!sendMQTTPingResponseMessage(connectionSocket)) {
        logger << logWarnMQTT << "Failed to send PINGRESP message to client " << clientID
               << ". Closing connection." << eol;
        handleConnectionSendFailure();
    }
}

void MQTTSession::disconnectMessageReceived(MQTTMessage &message) {
    MQTTDisconnectMessage disconnectMessage(message);

    // We do this for the log message, the connection is going the way of the water buffalo either
    // way.
    if (!disconnectMessage.parse()) {
        logger << logErrorMQTT << "Bad MQTT DISCONNECT message. Terminating connection." << eol;
        shutdown();
        return;
    }

    logger << logDebugMQTT << "Stopping client due to DISCONNECT" << eol;
    shutdown();
}

void MQTTSession::serverOnlyMsgReceivedError(MQTTMessage &message) {
    logger << logErrorMQTT << "Received server->client only message " << message.messageTypeStr()
           << " from client " << clientID << ". Terminating connection." << eol;
    shutdown();
}

void MQTTSession::reservedMsgReceivedError(MQTTMessage &message) {
    logger << logErrorMQTT << "Received reserved message " << message.messageTypeStr()
           << " from client " << clientID << ". Terminating connection." << eol;
    shutdown();
}

void MQTTSession::publish(const char *topic, const char *value, bool retainedValue) {
    logger << logDebugMQTT << "Publishing Topic '" << topic << "' to Client '" << clientID
           << "' with value '" << value << "' and retain " << retainedValue << eol;

    if (_connection != nullptr) {
        sendMQTTPublishMessage(connectionSocket, topic, value, false, 0, retainedValue, 0);
    }
}

const etl::istring &MQTTSession::name() const {
    return clientID;
}

void MQTTSession::resetKeepAliveTimer() {
    // *** Implement! ***
}

void MQTTSession::handleConnectionSendFailure() {
    // Signal the Connection that we got hung up on and make sure we stop using it
    _connection->markForDisconnection();
    _connection = nullptr;
    connectionSocket = 0;

    // MQTT's clean session flag on a connect indicates that the session should stay open after the
    // loss of a tcp connection. If the flag was off when the connection was made, we should stick
    // around on the broker's set of sessions that can be reconnected with. Otherwise we should go
    // idle for later reuse.
    if (!cleanSession) {
        broker.sessionLostConnection(*this);
    } else {
        broker.sessionGoingIdle(*this);
    }
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

// Called from a Connection thread
void MQTTSession::notifyConnectionLost() {
    if (xTaskNotifyIndexed(taskHandle(), notifyIndex, notifyConnectionLostMask,
                           eSetBits) != pdPASS) {
        taskLogger() << logErrorMQTT << "Failed to send connection lost notification to session #"
                     << id << eol;
        errorExit();
    }
}
