/*
 * This file is part of LunaMon (https://github.com/LisaRowell/LunaMon)
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

class MQTTBroker : public TaskObject, WiFiManagerClient {
    private:
        static constexpr size_t stackSize = 8 * 1024;
        static constexpr size_t maxMQTTConnections = CONFIG_LUNAMON_MAX_MQTT_CLIENTS;
        static constexpr uint16_t serverPort = 1883;
        static constexpr uint32_t lockTimeoutMs = 60 * 1000;

        int serverSocket;
        SemaphoreHandle_t connectionLock;
        etl::intrusive_list<MQTTConnection, ConnectionLink> idleConnections;
        etl::intrusive_list<MQTTConnection, ConnectionLink> activeConnections;
        SemaphoreHandle_t sessionLock;
        etl::intrusive_list<MQTTSession, SessionLink> freeSessions;
        etl::intrusive_list<MQTTSession, SessionLink> disconnectedSessions;

        virtual void task() override;
        void createServerSocket();
        void newConnection(int connectionSocket, struct sockaddr_in &sourceAddr,
                           socklen_t sourceAddrLength);
        void closeConnectionSocket(int connectionSocket);
        void takeConnectionLock();
        void releaseConnectionLock();

    public:
        MQTTBroker(WiFiManager &wifiManager);
        void connectionGoingIdle(MQTTConnection &connection);

};

#endif //MQTT_BROKER_H
