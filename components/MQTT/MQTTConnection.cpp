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

#include "MQTTConnection.h"
#include "MQTTBroker.h"
#include "MQTTMessage.h"
#include "MQTTConnectMessage.h"
#include "MQTTConnectAckMessage.h"
#include "MQTTString.h"

#include "Logger.h"
#include "Error.h"

#include "etl/string.h"
#include "etl/string_stream.h"

#include <lwip/sockets.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/message_buffer.h>

#include <strings.h>
#include <arpa/inet.h>

MQTTConnection::MQTTConnection(MQTTBroker &broker, uint8_t id)
    : TaskObject("MQTTConnection", LOGGER_LEVEL_DEBUG, stackSize),
      _id(id), broker(broker), connectionSocket(0) {
    bzero(&sourceAddr, sizeof(sourceAddr));

    sessionMessages = xMessageBufferCreate(sessionMessageBufferSize);
    if (sessionMessages == nullptr) {
        logger << logErrorMQTT << "Failed to allocation session message buffer for session #" << id
               << eol;
        errorExit();
    }
}

// This method is invoked by the Broker task and uses FreeRTOS's direct to task notification
// mechanism to wake up the connection's thread and fire it off on handling client messages. It's
// important that the connection be moved off of the idle list and is on the active thread list
// before this is invoked so that there isn't a race condition with a DOA socket.
void MQTTConnection::assignSocket(int connectionSocket, struct sockaddr_in &sourceAddr) {
    // It's important (or at least proper) that taskLogger() is used and not logger as this is not
    // invoked on the Connection's thread.
    taskLogger() << logDebugMQTT << "Assigning socket " << connectionSocket << " to Connection #"
                 << _id << eol;

    this->sourceAddr = sourceAddr;

    if (xTaskNotifyIndexed(taskHandle(), notifyIndex, (uint32_t)connectionSocket,
                           eSetValueWithOverwrite) != pdPASS) {
        logger << logErrorMQTT << "Failed to assign socket to Connection #" << _id << eol;
        errorExit();
    }
}

// This method is invoked by either a session that has received a disconnect or a connection which
// is taking over a session that's still paired with a now obsolete connection. Use the task's
// notification as a mailbox. After the next read or timeout, the task will pick it up and close.
void MQTTConnection::markForDisconnection() {
     if (xTaskNotifyIndexed(taskHandle(), notifyIndex, notifyDisconnect, eSetBits) != pdPASS) {
        logger << logErrorMQTT << "Failed to disconnect notification to Connection #" << _id << eol;
        errorExit();
    }
}

void MQTTConnection::waitForSocketAssignment() {
    // We loop here for correctness as it's theoretically possible to timeout even with a max wait
    // interval.
    BaseType_t socketReceived;
    uint32_t socketNotification;
    do {
        socketReceived = xTaskNotifyWaitIndexed(notifyIndex, 0, ULONG_MAX, &socketNotification,
                                                portMAX_DELAY);
    } while (!socketReceived);

    // Sanity check the socket number
    if (socketNotification == notifyDisconnect) {
        logger << logErrorMQTT << "MQTT Connection #" << _id
               << " received a disconnect notification while not connected." << eol;
        errorExit();
    }

    connectionSocket = (int)socketNotification;
    if (connectionSocket <= 0) {
        logger << logErrorMQTT << "MQTT Connection #" << _id
               << " was assigned an illegal socket number " << connectionSocket << eol;
        errorExit();
    }
}

uint8_t MQTTConnection::id() const {
    return _id;
}

const etl::istring &MQTTConnection::clientID() const {
    return _clientID;
}

int MQTTConnection::socket() const {
    return connectionSocket;
}

MessageBufferHandle_t MQTTConnection::sessionMessageBuffer() {
    return sessionMessages;
}

void MQTTConnection::task() {
    while (true != false) {
        waitForSocketAssignment();
        setSocketKeepalive();

        // The connection may be for an existing session, but we won't know that until we get a
        // CONNECT message. For now, mark as not having a session.
        session = nullptr;

        for (bool connectionOpen = true; connectionOpen; ) {
            MQTTMessage message;
            connectionOpen = readMessage(message);
            if (connectionOpen) {
                processMessage(message);
            }
        }

        shutdownConnection();
        waitForSessionDisconnect();
        goIdle();
    }
}

bool MQTTConnection::readMessage(MQTTMessage &message) {
    // MQTT has a leading header, called the Fixed Header, which has a variable length encoding of
    // the number of bytes coming after the fixed header. This can be used to calculate the overall
    // message length. Since this fixed header isn't actually of fixed length, and instead has a
    // funky, variable length, remaining bytes field, we need to read in the minimum header size,
    // see if it's enough to determine the length, if not, keep reading, byte, by byte, until we
    // know the length.

    // First read in the minimum header size. This may or may not be enough to determine the
    // message size.
    bytesInBuffer = 0;
    if (!readToBuffer(minMQTTFixedHeaderSize)) {
        return false;
    }

    MQTTFixedHeader *fixedHeader = (MQTTFixedHeader *)buffer;
    size_t fixedHeaderLength = minMQTTFixedHeaderSize;

    // If the minimum header didn't have the full length field, we repeatedly read in bytes until
    // we get a length byte with the end of length bit set.
    while (!messageRemainingLengthIsComplete(fixedHeader, fixedHeaderLength)) {
        if (!readToBuffer(1)) {
            return false;
        }
        fixedHeaderLength++;
    }

    size_t remainingLength;
    if (!determineRemainingLength(remainingLength, fixedHeader, fixedHeaderLength)) {
        logIllegalRemainingLength(fixedHeader, fixedHeaderLength);
        return false;
    }

    // While the MQTT protocol supports messages of 256 Mb, it's just not practical to support that
    // on an ESP32. We have a fixed length receive buffer; if a client tries to send a message
    // longer than that, thank and excuse the client.
    if (fixedHeaderLength + remainingLength > maxIncomingMessageSize) {
        logMessageSizeTooLarge(fixedHeaderLength + remainingLength);
        return false;
    }

    if (!readToBuffer(remainingLength)) {
        return false;
    }

    message = MQTTMessage(fixedHeader, fixedHeaderLength, remainingLength);
    return true;
}

bool MQTTConnection::processMessage(const MQTTMessage &message) {
    logger << logDebugMQTT << message.messageTypeStr() << " message received on connection #"
           << _id << " (" << sourceAddr << ")" << eol;

    // We process messages at the connection level until we successfully associate the connection
    // with either a pre-existing session or a new one. After that, we let the session handle the
    // message.
    if (!session) {
        switch (message.messageType()) {
            case MQTT_MSG_CONNECT:
                return processConnectMessage(message);

            default:
                logger << logWarnMQTT << "Unimplemented message type " << message.messageTypeStr()
                       << " message received from connection #" << _id << " (" << sourceAddr << ")"
                       << eol;
                return true;
        }
    } else {
        queueMessageForSession(message);
        return true;
    }
}

// Returns true if the message was processed without a terminating condition
bool MQTTConnection::processConnectMessage(const MQTTMessage &message) {
    MQTTConnectMessage connectMessage(message);
    if (!connectMessage.parse()) {
        logger << logWarnMQTT << "Bad connect message. Terminating connection." << eol;
        return false;
    }

    uint8_t errorCode;
    errorCode = connectMessage.sanityCheck();
    if (errorCode != MQTT_CONNACK_ACCEPTED) {
        logger << logWarnMQTT << "Terminating connection due to failed CONNECT message sanity check"
               << eol;
        // While we're refusing the connection, we don't terminate here and instead let it close out
        // naturally as we want the client to receive the NACK.
        return sendMQTTConnectAckMessage(connectionSocket, false, errorCode);
    }

    // Since this is a light weight broker, and a work in progress, we reject a few currently
    // unsupported types of connections.
    if (connectMessage.hasWill()) {
        logger << logWarnMQTT << "MQTT CONNECT with Will: Currently unsupported" << eol;
        return sendMQTTConnectAckMessage(connectionSocket, false,
                                         MQTT_CONNACK_REFUSED_SERVER_UNAVAILABLE);
    }
    if (connectMessage.hasUserName()) {
        logger << logWarnMQTT << "MQTT CONNECT message with Password set" << eol;
        return sendMQTTConnectAckMessage(connectionSocket, false,
                                         MQTT_CONNACK_REFUSED_USERNAME_OR_PASSWORD);
    }
    if (connectMessage.hasPassword()) {
        logger << logWarnMQTT << "MQTT CONNECT message with Password set" << eol;
        return sendMQTTConnectAckMessage(connectionSocket, false,
                                         MQTT_CONNACK_REFUSED_USERNAME_OR_PASSWORD);
    }

    const MQTTString *clientIDStr = connectMessage.clientID();
    if (!clientIDStr->copyTo(_clientID)) {
        logger << logWarnMQTT << "MQTT CONNECT message with too long of a Client ID:"
               << *clientIDStr << eol;
        return sendMQTTConnectAckMessage(connectionSocket, false,
                                         MQTT_CONNACK_REFUSED_IDENTIFIER_REJECTED);
    }

    // It's less than helpful to have a client connecting without providing a Client ID, though it
    // is allowed by the spec iff the clean session flag is set. We give such connections a name
    // based on their IP address and TCP port so that we can better debug things. It's best to
    // configure the offending application and have it provide sonmething useful...
    if (_clientID.empty()) {
        if (!connectMessage.cleanSession()) {
            logger << logWarnMQTT << "MQTT CONNECT message with a null Client ID and clean session"
                                     " false. Rejecting." << eol;
            return sendMQTTConnectAckMessage(connectionSocket, false,
                                             MQTT_CONNACK_REFUSED_IDENTIFIER_REJECTED);
        }

        etl::string_stream clientIDStream(_clientID);
    
        char connectionIPAddressStr[16];
        inet_ntoa_r(sourceAddr.sin_addr.s_addr, connectionIPAddressStr,
                    sizeof(connectionIPAddressStr) - 1);

        clientIDStream << connectionIPAddressStr << ":" << ntohs(sourceAddr.sin_port);
    }

    keepAliveTime = connectMessage.keepAliveSec();
    cleanSession = connectMessage.cleanSession();

    // Pair the connection with a session (existing or new) then, if successful queue the connect
    // message for it.
    session = broker.pairConnectionWithSession(this, cleanSession);
    if (session != nullptr) {
        logger << logDebugMQTT << "Paired connection (#" << _id << ") from client ID " << _clientID
               << " with session" << eol;

        queueMessageForSession(connectMessage);

        return true;
    } else {
        // We failed to pair with either an existing session or a free one. This could happen if
        // we were full up with either active sessions or ones waiting for a reconnection and we
        // got some new connection for a completely new client. In this case we want to make sure
        // and clear out the session message queue.
        logger << logWarnMQTT << "Failed to get a session for connection #" << _id << " ("
               << sourceAddr << ")" << eol;
        clearSessionMessages();
        return sendMQTTConnectAckMessage(connectionSocket, false,
                                         MQTT_CONNACK_REFUSED_SERVER_UNAVAILABLE);
    }

    return true;
}

// An MQTT message has a variable length field indicating the remaining length of the message after
// the first header. This is encoded as a series of 1-4 bytes with the MSB of the last byte being 0
// with any preceeding bytes having an MSB of 1. This routine requires the caller to call it for
// each byte read in.
bool MQTTConnection::messageRemainingLengthIsComplete(MQTTFixedHeader *fixedHeader,
                                                      size_t fixedHeaderLength) {
    if (fixedHeaderLength == maxMQTTFixedHeaderSize) {
        return true;
    } else {
        const unsigned lastLengthByteIndex = fixedHeaderLength - minMQTTFixedHeaderSize;
        return (fixedHeader->remainingLength[lastLengthByteIndex] & 0x80) == 0;
    }
}

bool MQTTConnection::determineRemainingLength(size_t &remainingLength, MQTTFixedHeader *fixedHeader,
                                              size_t fixedHeaderLength) {
    const unsigned remainingLengthBytes = fixedHeaderLength - sizeof(fixedHeader->typeAndFlags);
    unsigned lengthByte;
    remainingLength = 0;
    uint32_t multiplier = 1;
    for (lengthByte = 0, multiplier = 1, remainingLength = 0;
         lengthByte < remainingLengthBytes;
         lengthByte++, multiplier = multiplier * 128) {
        remainingLength += fixedHeader->remainingLength[lengthByte] & 0x7f * multiplier;
    }

    if (fixedHeader->remainingLength[remainingLengthBytes - 1] & 0x80) {
        // Per the MQTT specification, the last remaining length byte must have a 0 MSB.
        return false;
    } else {
        return true;
    }
}

void MQTTConnection::setSocketKeepalive() {
    int keepAlive = 1;
    int keepIdle = CONFIG_LUNAMON_TCP_KEEPALIVE_IDLE;
    int keepInterval = CONFIG_LUNAMON_TCP_KEEPALIVE_INTERVAL;
    int keepCount = CONFIG_LUNAMON_TCP_KEEPALIVE_COUNT;

    setsockopt(connectionSocket, SOL_SOCKET, SO_KEEPALIVE, &keepAlive, sizeof(int));
    setsockopt(connectionSocket, IPPROTO_TCP, TCP_KEEPIDLE, &keepIdle, sizeof(int));
    setsockopt(connectionSocket, IPPROTO_TCP, TCP_KEEPINTVL, &keepInterval, sizeof(int));
    setsockopt(connectionSocket, IPPROTO_TCP, TCP_KEEPCNT, &keepCount, sizeof(int));
}

bool MQTTConnection::readToBuffer(size_t readAmount) {
    // Since lwip's recv does not currently implement the MSG_WAITALL flag, we loop until we receive
    // all required bytes or encounter an error or connection closure.
    while (readAmount) {
        ssize_t bytesRead = recv(connectionSocket, buffer + bytesInBuffer, readAmount, 0);
        if (bytesRead < 0) {
            return false;
        }

        bytesInBuffer += bytesRead;
        readAmount -= bytesRead;
    }

    return true;
}

void MQTTConnection::shutdownConnection() {
    logger << "Shutting down Connection #" << _id << " (" << sourceAddr << ")" << eol;

    // Properly shutdown the socket
    shutdown(connectionSocket, SHUT_WR);
    shutdown(connectionSocket, SHUT_RD);
    close(connectionSocket);

    // Move ourselves to the idle list
    broker.connectionGoingIdle(*this);
}

void MQTTConnection::waitForSessionDisconnect() {
    // We loop here for correctness as it's theoretically possible to timeout even with a max wait
    // interval.
    BaseType_t notificationReceived;
    uint32_t notification;
    do {
        notificationReceived = xTaskNotifyWaitIndexed(notifyIndex, 0, ULONG_MAX, &notification,
                                                      portMAX_DELAY);
    } while (!notificationReceived);

    // Sanity check the notification
    if (notification != notifyDisconnect) {
        logger << logErrorMQTT << "MQTT Connection #" << _id
               << " received a socket notification while waiting for a disconnect." << eol;
        errorExit();
    }

    logger << logDebugMQTT << "MQTT Connection #" << _id << " received disconnect." << eol;
}

void MQTTConnection::goIdle() {
    logger << logDebugMQTT << "MQTT Connection #" << _id << " going idle." << eol;

    session = nullptr;

    // Move ourselves to the idle list
    broker.connectionGoingIdle(*this);
}

void MQTTConnection::queueMessageForSession(const MQTTMessage &message) {
    if (xMessageBufferSend(sessionMessages, message.messageStart(), message.totalLength(),
                           sessionMessageBufferTimeout) == 0) {
        logger << logErrorMQTT << "Failed to enqueue MQTT " << message.messageTypeStr()
               << " message from connection #" << _id << " (" << sourceAddr << ") to session"
               << eol;
        errorExit();
    }

    // Since sessions wait both on a queue and on notifications, send the session a notification
    // that has a message waiting for it.
    session->notifyMessageReady();
}

void MQTTConnection::clearSessionMessages() {
    if (xMessageBufferReset(sessionMessages) != pdPASS) {
        logger << logErrorMQTT << "Failed to reset session message buffer for connection #" << _id
               << eol;
        errorExit();
    }
}

void MQTTConnection::logIllegalRemainingLength(MQTTFixedHeader *fixedHeader,
                                               size_t fixedHeaderLength) {
    logger << logNotifyMQTT <<  "Illegal MQTT message remaining length: " << Hex;

    const unsigned remainingLengthBytes = fixedHeaderLength - sizeof(fixedHeader->typeAndFlags); 
    for (unsigned lengthByte = 0; lengthByte < remainingLengthBytes; lengthByte++) {
        logger << fixedHeader->remainingLength[lengthByte];
    }

    logger << " Aborting connection #" << _id << " (" << sourceAddr << ")" << eol;
}

void MQTTConnection::logMessageSizeTooLarge(size_t messageSize) {
    logger << logErrorMQTT << "Message size " << messageSize << " from " << sourceAddr
           << " exceeds maximum allowable ("
           << maxIncomingMessageSize << "). Aborting connection." << eol;
}
