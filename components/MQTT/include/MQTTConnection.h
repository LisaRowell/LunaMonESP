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

#ifndef MQTT_CONNECTION_H
#define MQTT_CONNECTION_H

#include "TaskObject.h"

#include "MQTTMessage.h"

#include "etl/intrusive_links.h"

#include <lwip/sockets.h>

class MQTTBroker;
class MQTTSession;

constexpr size_t connectionLinkId = 0;
typedef etl::bidirectional_link<connectionLinkId> ConnectionLink;

class MQTTConnection : public TaskObject, public ConnectionLink {
    private:
        static constexpr size_t stackSize = 8 * 1024;

        static constexpr size_t minMQTTFixedHeaderSize = 2;
        static constexpr size_t maxMQTTFixedHeaderSize = (minMQTTFixedHeaderSize + 3);
        static constexpr uint32_t maxIncomingMessageSize = 1024;

        static constexpr size_t maxMQTTClientIDLength = 23;

        uint8_t id;
        MQTTBroker &broker;
        int connectionSocket;
        struct sockaddr_in sourceAddr;
        MQTTSession *session;
        uint8_t buffer[maxIncomingMessageSize];
        size_t bytesInBuffer;

        virtual void task() override;
        void waitForSocketAssignment();
        void setSocketKeepalive();
        bool readMessage(MQTTMessage &message);
        bool processMessage(const MQTTMessage &message);
        bool processConnectMessage(const MQTTMessage &message);
        bool messageRemainingLengthIsComplete(MQTTFixedHeader *fixedHeader,
                                              size_t fixedHeaderLength);
        bool determineRemainingLength(size_t &remainingLength, MQTTFixedHeader *fixedHeader,
                                      size_t fixedHeaderLength);
        bool readToBuffer(size_t readAmount);
        void shutdownConnection();
        void logIllegalRemainingLength(MQTTFixedHeader *fixedHeader, size_t fixedHeaderLength);
        void logMessageSizeTooLarge(size_t messageSize);

    public:
        MQTTConnection(MQTTBroker &broker, uint8_t id);
        void assignSocket(int connectionSocket, struct sockaddr_in &sourceAddr);
};

#endif // MQTT_CONNECTION_H
