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

#ifndef MQTT_SESSION_H
#define MQTT_SESSION_H

#include "TaskObject.h"

#include "MQTT.h"

#include "etl/intrusive_links.h"
#include "etl/string.h"

#include <freertos/FreeRTOS.h>
#include <freertos/message_buffer.h>

class MQTTBroker;
class MQTTConnection;
class MQTTMessage;

constexpr size_t sessionLinkId = 0;
typedef etl::bidirectional_link<sessionLinkId> SessionLink;

class MQTTSession : public TaskObject, public SessionLink {
    private:
        static constexpr size_t stackSize = 8 * 1024;

        // Since we use freeRtos message buffers, we can't safely use the OG notification index of
        // 0. Use our own index instead.
        static constexpr UBaseType_t notifyIndex = 1;

        static constexpr uint32_t notifyNewConnectionIdMask  = 0xf0000000;
        static constexpr unsigned notifyNewConnectionIdShift = 28;
        static constexpr uint32_t notifyShutdownMask         = 0x08000000;
        static constexpr uint32_t notifyMessageReadyMask     = 0x04000000;
        static constexpr uint32_t notifyConnectionLostMask   = 0x02000000;

        static constexpr uint32_t maxIncomingMessageSize = 1024;

        uint8_t id;
        MQTTBroker &broker;
        etl::string<maxMQTTClientIDLength> clientID;
        MQTTConnection *_connection;
        bool cleanSession;
        bool freshSession;
        int connectionSocket;
        MessageBufferHandle_t connectionMessageBuffer;

        virtual void task() override;
        void newConnection(unsigned connectionId);
        void cancelPendingConnectionAssignment();
        void readMessages();
        void handleConnectMessage(MQTTMessage &message);
        void handleConnectionSendFailure();
        void shutdown();
        void connectionLost();

    public:
        MQTTSession(MQTTBroker &broker, uint8_t id);
        void assignConnection(unsigned connectionId);
        void initiateShutdown();
        bool isForClient(const etl::istring &clientID) const;
        MQTTConnection *connection() const;
        void notifyMessageReady();
        void notifyConnectionLost();
};

#endif //MQTT_SESSION_H
