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

#include "etl/string_stream.h"

#include <lwip/sockets.h>

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <strings.h>
#include <arpa/inet.h>

MQTTConnection::MQTTConnection(MQTTBroker &broker, uint8_t id)
    : TaskObject("MQTTConnection", LOGGER_LEVEL_DEBUG, stackSize),
      id(id), broker(broker), connectionSocket(0) {
    bzero(&sourceAddr, sizeof(sourceAddr));
}

// This method is invoked by the Broker task and uses FreeRTOS's direct to task notification
// mechanism to wake up the connection's thread and fire it off on handling client messages. It's
// important that the connection be moved off of the idle list and is on the active thread list
// before this is invoked so that there isn't a race condition with a DOA socket.
void MQTTConnection::assignSocket(int connectionSocket, struct sockaddr_in &sourceAddr) {
    // It's important (or at least proper) that taskLogger() is used and not logger as this is not
    // invoked on the Connection's thread.
    taskLogger() << logDebugMQTT << "Assigning socket " << connectionSocket << " to Connection #"
                 << id << eol;

    this->sourceAddr = sourceAddr;

    (void)xTaskNotify(taskHandle(), (uint32_t)connectionSocket, eSetValueWithOverwrite);
}

void MQTTConnection::waitForSocketAssignment() {
    // We loop here for correctness as it's theoretically possible to timeout even with a max wait
    // interval.
    BaseType_t socketReceived;
    uint32_t socketNotification;
    do {
        socketReceived = xTaskNotifyWait(0, ULONG_MAX, &socketNotification,
                                                    portMAX_DELAY);
    } while (!socketReceived);

    // Sanity check the socket number
    connectionSocket = (int)socketNotification;
    if (connectionSocket <= 0) {
        logger << logErrorMQTT << "MQTT Connection was assigned an illegal socket number "
               << connectionSocket << eol;
        errorExit();
    }
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
    logger << logDebugMQTT << message.messageTypeStr() << " message received from connection #"
           << id << " (" << sourceAddr << ")" << eol;

    switch (message.messageType()) {
        case MQTT_MSG_CONNECT:
            return processConnectMessage(message);

        default:
            logger << logWarnMQTT << "Unimplemented message type " << message.messageTypeStr()
                   << " message received from connection #" << id << " (" << sourceAddr << ")"
                   << eol;
            return true;
    }
}

// Returns true if the message was processed without a terminating condition
bool MQTTConnection::processConnectMessage(const MQTTMessage &message) {
    // Per the MQTT specification, we treat a second CONNECT for a connection as a protocol error.
    if (session) {
        // Todo: Make this message more detailed to aid in debugging.
        logger << logWarnMQTT << "Second MQTT CONNECT received for a connection." << eol;
        return false;
    }

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
        sendMQTTConnectAckMessage(connectionSocket, false, errorCode);
        // While we're refusing the connection, we don't terminate here and instead let it close out
        // naturally as we want the client to receive the NACK.
        return true;
    }

    // Since this is a light weight broker, and a work in progress, we reject a few currently
    // unsupported types of connections.
    if (connectMessage.hasWill()) {
        logger << logWarnMQTT << "MQTT CONNECT with Will: Currently unsupported" << eol;
        sendMQTTConnectAckMessage(connectionSocket, false, MQTT_CONNACK_REFUSED_SERVER_UNAVAILABLE);
        return true;
    }
    if (connectMessage.hasUserName()) {
        logger << logWarnMQTT << "MQTT CONNECT message with Password set" << eol;
        sendMQTTConnectAckMessage(connectionSocket, false,
                                  MQTT_CONNACK_REFUSED_USERNAME_OR_PASSWORD);
        return true;
    }
    if (connectMessage.hasPassword()) {
        logger << logWarnMQTT << "MQTT CONNECT message with Password set" << eol;
        sendMQTTConnectAckMessage(connectionSocket, false,
                                  MQTT_CONNACK_REFUSED_USERNAME_OR_PASSWORD);
        return true;
    }

    const MQTTString *clientIDStr = connectMessage.clientID();
    etl::string<maxMQTTClientIDLength> clientID;
    if (!clientIDStr->copyTo(clientID)) {
        logger << logWarnMQTT << "MQTT CONNECT message with too long of a Client ID:"
               << *clientIDStr << eol;
        sendMQTTConnectAckMessage(connectionSocket, false,
                                  MQTT_CONNACK_REFUSED_IDENTIFIER_REJECTED);
        return true;
    }

    // It's less than helpful to have a client connecting without providing a Client ID, though it
    // is allowed by the spec iff the clean session flag is set. We give such connections a name
    // based on their IP address and TCP port so that we can better debug things. It's best to
    // configure the offending application and have it provide sonmething useful...
    if (clientID.empty()) {
        etl::string_stream clientIDStream(clientID);
    
        char connectionIPAddressStr[16];
        inet_ntoa_r(sourceAddr.sin_addr.s_addr, connectionIPAddressStr,
                    sizeof(connectionIPAddressStr) - 1);

        clientIDStream << connectionIPAddressStr << ":" << ntohs(sourceAddr.sin_port);
    }

    uint16_t keepAliveTime = connectMessage.keepAliveSec();

    // Add session stuff here
    return sendMQTTConnectAckMessage(connectionSocket, false, MQTT_CONNACK_ACCEPTED);

//    return true;
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
    logger << "Shutting down Connection #" << id << " (" << sourceAddr << ")" << eol;

    // Properly shutdown the socket
    shutdown(connectionSocket, SHUT_WR);
    shutdown(connectionSocket, SHUT_RD);
    close(connectionSocket);

    // Move ourselves to the idle list
    broker.connectionGoingIdle(*this);
}

void MQTTConnection::logIllegalRemainingLength(MQTTFixedHeader *fixedHeader,
                                               size_t fixedHeaderLength) {
    logger << logNotifyMQTT <<  "Illegal MQTT message remaining length: " << Hex;

    const unsigned remainingLengthBytes = fixedHeaderLength - sizeof(fixedHeader->typeAndFlags); 
    for (unsigned lengthByte = 0; lengthByte < remainingLengthBytes; lengthByte++) {
        logger << fixedHeader->remainingLength[lengthByte];
    }

    logger << " Aborting connection #" << id << " (" << sourceAddr << ")" << eol;
}

void MQTTConnection::logMessageSizeTooLarge(size_t messageSize) {
    logger << logErrorMQTT << "Message size " << messageSize << " from " << sourceAddr
           << " exceeds maximum allowable ("
           << maxIncomingMessageSize << "). Aborting connection." << eol;
}
