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

#include "WiFiManager.h"

#include "TaskObject.h"
#include "Logger.h"
#include "ESPError.h"
#include "Error.h"

#include "esp_netif.h"
#include "esp_event.h"
#include "esp_event_base.h"
#include "esp_wifi.h"
#include "esp_wifi_default.h"
#include "esp_err.h"

#include "freertos/FreeRTOSConfig.h"
#include "freertos/event_groups.h"
#include "freertos/portmacro.h"

#include <stdint.h>
#include <string.h>

WiFiManager::WiFiManager()
    : TaskObject("WiFi Manager", LOGGER_LEVEL_DEBUG, stackSize), connected(false) {
    logger.enableModuleDebug(LOGGER_MODULE_WIFI_MANAGER);
}

void WiFiManager::start() {
    esp_err_t error;

    eventGroup = xEventGroupCreate();

    if ((error = esp_netif_init()) != ESP_OK) {
        logger << logErrorWiFiManager << "Failed to initialize ESP netif: " << ESPError(error)
               << eol;
        errorExit();
    }

    if ((error = esp_event_loop_create_default()) != ESP_OK) {
        logger << logErrorWiFiManager << "Failed to create default event loop: " << ESPError(error)
               << eol;
        errorExit();
    }

    esp_netif_create_default_wifi_sta();

    wifi_init_config_t wifiInitConfig = WIFI_INIT_CONFIG_DEFAULT();
    if ((error = esp_wifi_init(&wifiInitConfig)) != ESP_OK) {
        logger << logErrorWiFiManager << "Failed to init WiFi: " << ESPError(error)
               << eol;
        errorExit();
    }

    if ((error = esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &eventHandler,
                                                     this, NULL)) != ESP_OK) {
        logger << logErrorWiFiManager << "Failed to register event handler for WiFi events: "
               << ESPError(error) << eol;
        errorExit();
    }
    if ((error = esp_event_handler_instance_register(IP_EVENT, IP_EVENT_STA_GOT_IP, &eventHandler,
                                                     this, NULL)) != ESP_OK) {
        logger << logErrorWiFiManager << "Failed to register event handler for got IP event: "
               << ESPError(error) << eol;
        errorExit();
    }

    if ((error = esp_wifi_set_mode(WIFI_MODE_STA)) != ESP_OK) {
        logger << logErrorWiFiManager << "Failed to set WiFi station mode: " << ESPError(error)
               << eol;
        errorExit();
    }

    wifi_config_t wifiConfig;
    memset(&wifiConfig, 0, sizeof(wifi_config_t));
    strncpy((char *)wifiConfig.sta.ssid, CONFIG_WIFI_SSID, sizeof(wifiConfig.sta.ssid));
    strncpy((char *)wifiConfig.sta.password, CONFIG_WIFI_PASSWORD, sizeof(wifiConfig.sta.password));
    if ((error = esp_wifi_set_config(WIFI_IF_STA, &wifiConfig)) != ESP_OK) {
        logger << logErrorWiFiManager << "Failed to set WiFi station mode: " << ESPError(error)
               << eol;
        errorExit();
    }

    if ((error = esp_wifi_start()) != ESP_OK) {
        logger << logErrorWiFiManager << "WiFi start failed: " << ESPError(error) << eol;
        errorExit();
    }

    logger << logDebugWiFiManager << "WiFi initialization complete" << eol;

    TaskObject::start();
}

void WiFiManager::eventHandler(void *wifiManagerPtr, esp_event_base_t eventBase, int32_t eventId,
                               void *eventData) {
    WiFiManager *wifiManager = (WiFiManager *)wifiManagerPtr;

    if (eventBase == WIFI_EVENT) {
        wifiManager->handleWiFiEvent(eventId, eventData);
    } else if (eventBase == IP_EVENT) {
        wifiManager->handleIPEvent(eventId, eventData);
    } else {
        // Error debug here
    }
}

void WiFiManager::handleWiFiEvent(int32_t eventId, void *eventData) {
    (void)eventData;

    switch (eventId) {
        case WIFI_EVENT_STA_START:
            esp_wifi_connect();
            break;
        
        case WIFI_EVENT_STA_DISCONNECTED:
            xEventGroupClearBits(eventGroup, WIFI_CONNECTED_BIT);
            xEventGroupSetBits(eventGroup, WIFI_FAIL_BIT);
            break;
    }
}

void WiFiManager::handleIPEvent(int32_t eventId, void *eventData) {
    (void)eventData;

    switch (eventId) {
        case IP_EVENT_STA_GOT_IP:
            xEventGroupClearBits(eventGroup, WIFI_FAIL_BIT);
            xEventGroupSetBits(eventGroup, WIFI_CONNECTED_BIT);
            break;

        default:
            // This is problematic since logger is used by the task and this is on a different
            // thread.
            logger << logWarnWiFiManager << "Unexpected IP event " << eventId << eol;
    }
}

void WiFiManager::task() {
    while (true != false) {
        EventBits_t eventBits = xEventGroupWaitBits(eventGroup, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                                    pdTRUE, pdFALSE, portMAX_DELAY);
        if (eventBits & WIFI_CONNECTED_BIT) {
            logger << logNotifyWiFiManager << "WiFi connected to SSID " << CONFIG_WIFI_SSID
                   << eol;
            connected = true;
        } else if (eventBits & WIFI_FAIL_BIT) {
            wifiDisconnection();
        } else {
            // Timeout of max delay
        }
    }
}

void WiFiManager::wifiDisconnection() {
    esp_err_t error;

    logger << logNotifyWiFiManager << "Connection to WiFi to SSID " << CONFIG_WIFI_SSID
           << " failed. Retrying..." << eol;

    connected = false;

    if ((error = esp_wifi_stop()) != ESP_OK) {
        logger << logWarnWiFiManager << "esp_wifi_stop failed: " << ESPError(error) << eol;
    }
    vTaskDelay(pdMS_TO_TICKS(connectionRetryTimeMs));
    if ((error = esp_wifi_start()) != ESP_OK) {
        logger << logWarnWiFiManager << "Call to restart WiFi via esp_wifi_start failed: "
               << ESPError(error) << eol;
        errorExit();
    }
}
