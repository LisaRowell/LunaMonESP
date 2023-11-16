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

#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include "TaskObject.h"

#include "esp_event_base.h"

#include "freertos/FreeRTOSConfig.h"
#include "freertos/event_groups.h"

#include <stddef.h>
#include <stdint.h>

#define WIFI_CONNECTED_BIT 0b00000001
#define WIFI_FAIL_BIT      0b00000010

class WiFiManager : TaskObject {
    private:
        static constexpr size_t stackSize = 8 * 1024;
        EventGroupHandle_t eventGroup;
        bool connected;

        virtual void task() override;
        void handleWiFiEvent(int32_t eventId, void *eventData);
        void handleIPEvent(int32_t eventId, void *eventData);

        static void eventHandler(void *arg, esp_event_base_t eventBase, int32_t eventId,
                                 void *eventData);

    public:
        WiFiManager();
        void start();
};

#endif // WIFI_MANAGER_H
