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

#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "TaskObject.h"

#include "esp_event_base.h"
#include "esp_netif.h"

#include "freertos/FreeRTOSConfig.h"
#include "freertos/event_groups.h"

#include <stddef.h>
#include <stdint.h>

#define WIFI_CONNECTED_EVENT            0b00000001
#define WIFI_DISCONNECTED_EVENT         0b00000010
#define WIFI_CONNECTION_FAILED_EVENT    0b00000100

class WiFiManager : TaskObject {
    private:
        static constexpr size_t stackSize = 8 * 1024;
        static constexpr uint32_t connectionRetryTimeMs = 1000;
        bool connected;
        esp_netif_t *wifiInterface;

        virtual void task() override;
        void handleWiFiEvent(int32_t eventId, void *eventData);
        void handleIPEvent(int32_t eventId, void *eventData);
        void wifiConnected();
        void wifiDisconnection();

        static void eventHandler(void *arg, esp_event_base_t eventBase, int32_t eventId,
                                 void *eventData);

    public:
        EventGroupHandle_t eventGroup;

        WiFiManager();
        void start();
};

#endif // WIFI_MANAGER_H
