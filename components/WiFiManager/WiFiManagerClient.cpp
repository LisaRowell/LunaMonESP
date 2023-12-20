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

#include "WiFiManagerClient.h"

#include "WiFiManager.h"

#include "freertos/FreeRTOSConfig.h"
#include "freertos/event_groups.h"

WiFiManagerClient::WiFiManagerClient(WiFiManager &wifiManager) : wifiManager(wifiManager) {
}

void WiFiManagerClient::waitForWiFiConnect() {
    EventBits_t eventBits;
    do {
        eventBits = xEventGroupWaitBits(wifiManager.eventGroup, WIFI_CONNECTED_EVENT, pdFALSE,
                                        pdFALSE, portMAX_DELAY);
    } while ((eventBits & WIFI_CONNECTED_EVENT) == 0);
}

bool WiFiManagerClient::wifiConnected() {
    EventBits_t eventBits = xEventGroupGetBits(wifiManager.eventGroup);
    return (eventBits & WIFI_CONNECTED_EVENT) != 0;
}
