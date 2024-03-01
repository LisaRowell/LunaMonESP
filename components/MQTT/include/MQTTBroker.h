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

#ifndef MQTT_BROKER_H
#define MQTT_BROKER_H

#include "MQTTConnection.h"
#include "MQTTSession.h"
#include "MQTTMessage.h"

#include "TaskObject.h"
#include "WiFiManagerClient.h"

#include "etl/intrusive_list.h"

#include <lwip/sockets.h>

#include <freertos/semphr.h>

class DataModel;

class MQTTBroker : public TaskObject, WiFiManagerClient {
    private:
        static constexpr size_t stackSize = 8 * 1024;
        static constexpr size_t maxMQTTConnections = CONFIG_LUNAMON_MAX_MQTT_CLIENTS;
        static constexpr size_t maxMQTTSessions = CONFIG_LUNAMON_MAX_MQTT_CLIENTS;
        static constexpr uint16_t serverPort = 1883;
        static constexpr uint32_t lockTimeoutMs = 60 * 1000;

        int serverSocket;
        SemaphoreHandle_t connectionLock;
        etl::intrusive_list<MQTTConnection, ConnectionLink> idleConnections;
        etl::intrusive_list<MQTTConnection, ConnectionLink> activeConnections;
        MQTTConnection *connectionIndex[maxMQTTConnections + 1];
        SemaphoreHandle_t sessionLock;
        etl::intrusive_list<MQTTSession, SessionLink> freeSessions;
        etl::intrusive_list<MQTTSession, SessionLink> activeSessions;
        etl::intrusive_list<MQTTSession, SessionLink> disconnectedSessions;

        virtual void task() override;
        void createServerSocket();
        void newConnection(int connectionSocket, struct sockaddr_in &sourceAddr,
                           socklen_t sourceAddrLength);
        void closeConnectionSocket(int connectionSocket);
        MQTTSession *pairConnectionWithCleanSession(MQTTConnection *connection);
        MQTTSession * pairConnectionWithNonCleanSession(MQTTConnection *connection);

        // It's important that if both the session and the connection locks are to be take, the
        // session lock must be taken first to avoid a starving philosopher scenerio.
        void takeConnectionLock();
        void releaseConnectionLock();
        void takeSessionLock();
        void releaseSessionLock();

    public:
        MQTTBroker(WiFiManager &wifiManager, DataModel &dataModel);
        MQTTConnection *connectionForId(uint8_t connectionId) const;
        void connectionGoingIdle(MQTTConnection &connection);
        MQTTSession *pairConnectionWithSession(MQTTConnection *connection, bool cleanSession);
        void sessionGoingIdle(MQTTSession &session);
        void sessionLostConnection(MQTTSession &session);
};

#endif //MQTT_BROKER_H
