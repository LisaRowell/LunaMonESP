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

#ifndef MQTT_SESSION_H
#define MQTT_SESSION_H

#include "DataModelSubscriber.h"

#include "TaskObject.h"

#include "MQTT.h"

#include "etl/intrusive_links.h"
#include "etl/string.h"

#include <freertos/FreeRTOS.h>
#include <freertos/message_buffer.h>

#include <stdint.h>

class MQTTBroker;
class MQTTConnection;
class MQTTMessage;
class DataModel;

constexpr size_t sessionLinkId = 0;
typedef etl::bidirectional_link<sessionLinkId> SessionLink;

class MQTTSession : public DataModelSubscriber, public TaskObject, public SessionLink {
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
        static constexpr uint32_t maxTopicsPerSubscribeMessage = 100;

        uint8_t id;
        MQTTBroker &broker;
        DataModel &dataModel;
        etl::string<maxMQTTClientIDLength> clientID;
        MQTTConnection *_connection;
        bool cleanSession;
        bool freshSession;
        int connectionSocket;
        MessageBufferHandle_t connectionMessageBuffer;
        uint32_t _messagesReceived;
        uint32_t _messagesSent;
        uint32_t _publishMessagesReceived;
        uint32_t _publishMessagesSent;
        uint32_t _publishMessagesDropped;

        virtual void task() override;
        void newConnection(unsigned connectionId);
        void cancelPendingConnectionAssignment();
        void readMessages();
        void connectMessageReceived(MQTTMessage &message);
        void subscribeMessageReceived(MQTTMessage &message);
        void unsubscribeMessageReceived(MQTTMessage &message);
        void pingRequestMessageReceived(MQTTMessage &message);
        void disconnectMessageReceived(MQTTMessage &message);
        void serverOnlyMsgReceivedError(MQTTMessage &message);
        void reservedMsgReceivedError(MQTTMessage &message);
        virtual void publish(const char *topic, const char *value, bool retainedValue) override;
        uint8_t subscribeResult(bool success, uint8_t maxQoS);
        bool sendSubscribeAckMessage(uint16_t packetId, uint8_t numberResults, uint8_t *results);
        bool sendUnsubscribeAckMessage(uint16_t packetId);
        bool sendPublishMessage(const char *topic, const char *value, bool dup, uint8_t qosLevel,
                                bool retain, uint16_t packetId);
        bool sendPingResponseMessage();
        virtual const etl::istring &name() const override;
        void resetKeepAliveTimer();
        void handleConnectionSendFailure();
        void shutdown();
        void connectionLost();

    public:
        MQTTSession(MQTTBroker &broker, DataModel &dataModel, uint8_t id);
        void assignConnection(unsigned connectionId);
        void initiateShutdown();
        const etl::istring &getClientID() const;
        bool isForClient(const etl::istring &clientID) const;
        MQTTConnection *connection() const;
        void notifyMessageReady();
        void notifyConnectionLost();
        uint32_t messagesReceived() const;
        uint32_t messagesSent() const;
        uint32_t publishMessagesReceived() const;
        uint32_t publishMessagesSent() const;
        uint32_t publishMessagesDropped() const;
};

#endif //MQTT_SESSION_H
