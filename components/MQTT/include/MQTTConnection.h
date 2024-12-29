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

#ifndef MQTT_CONNECTION_H
#define MQTT_CONNECTION_H

#include "TaskObject.h"

#include "MQTT.h"
#include "MQTTMessage.h"

#include "etl/string.h"
#include "etl/intrusive_links.h"

#include <freertos/FreeRTOS.h>
#include <freertos/message_buffer.h>

#include <lwip/sockets.h>

#include <stdint.h>

class MQTTBroker;
class MQTTSession;

constexpr size_t connectionLinkId = 0;
typedef etl::bidirectional_link<connectionLinkId> ConnectionLink;

class MQTTConnection : public TaskObject, public ConnectionLink {
    private:
        static constexpr size_t stackSize = 4 * 1024;

        static constexpr size_t minMQTTFixedHeaderSize = 2;
        static constexpr size_t maxMQTTFixedHeaderSize = (minMQTTFixedHeaderSize + 3);
        static constexpr uint32_t maxIncomingMessageSize = 1024;

        // Since we use freeRtos message buffers, we can't safely use the OG notification index of
        // 0. Use our own index instead.
        static constexpr UBaseType_t notifyIndex = 1;

        // Used as a notification when a session (or another connection) wants to force
        // disconnection of the client.
        static constexpr int notifyDisconnect = -1;

        uint8_t _id;
        MQTTBroker &broker;
        int connectionSocket;
        struct sockaddr_in sourceAddr;
        etl::string<maxMQTTClientIDLength> _clientID;

        MQTTSession *session;
        uint8_t buffer[maxIncomingMessageSize];
        size_t bytesInBuffer;

        // Currently we queue incoming messages in a FreeRTOS message buffer for an attached
        // MQTTSession to pull out and deal with. This has the annoying characteristic of an extra
        // copy from the incoming buffer to the outgoing, but it simplifies logic.
        // An alternative approach would be for the session to read messages after the connect
        // message itself.
        static constexpr size_t sessionMessageBufferSize = 2 * 1024;
        static constexpr TickType_t sessionMessageBufferTimeout = pdMS_TO_TICKS(10000);
        MessageBufferHandle_t sessionMessages;

        uint16_t keepAliveTime;
        bool cleanSession;
        uint32_t _messagesSent;

        virtual void task() override;
        void waitForSocketAssignment();
        void setSocketOptions();
        bool readMessage(MQTTMessage &message);
        bool processMessage(const MQTTMessage &message);
        bool processConnectMessage(const MQTTMessage &message);
        bool messageRemainingLengthIsComplete(MQTTFixedHeader *fixedHeader,
                                              size_t fixedHeaderLength);
        bool determineRemainingLength(size_t &remainingLength, MQTTFixedHeader *fixedHeader,
                                      size_t fixedHeaderLength);
        bool readToBuffer(size_t readAmount);
        void shutdownConnection();
        void waitForSessionDisconnect();
        void goIdle();
        void queueMessageForSession(const MQTTMessage &message);
        void clearSessionMessages();
        void logIllegalRemainingLength(MQTTFixedHeader *fixedHeader, size_t fixedHeaderLength);
        void logMessageSizeTooLarge(size_t messageSize);

    public:
        MQTTConnection(MQTTBroker &broker, uint8_t id);
        void assignSocket(int connectionSocket, struct sockaddr_in &sourceAddr);
        void markForDisconnection();
        uint8_t id() const;
        const etl::istring &clientID() const;
        int socket() const;
        MessageBufferHandle_t sessionMessageBuffer();
        uint32_t messagesSent();
};

#endif // MQTT_CONNECTION_H
