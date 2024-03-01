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

#include "LunaMon.h"

#include "WiFiManager.h"
#include "NMEAWiFiSource.h"
#include "StatusLED.h"
#include "I2CMaster.h"
#include "EnvironmentalMon.h"
#include "Logger.h"
#include "ESPError.h"
#include "Error.h"

#include "driver/gpio.h"
#include "driver/i2c.h"
#include "nvs_flash.h"
#include "esp_err.h"

#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"

#if !CONFIG_LUNAMON_NMEA_WIFI_ENABLED
#define CONFIG_LUNAMON_NMEA_WIFI_SOURCE_IPV4_ADDR   ""
#define CONFIG_LUNAMON_NMEA_WIFI_SOURCE_TCP_PORT    0
#endif

#if CONFIG_LUNAMON_STATUS_LED_ENABLED
#define STATUS_LED_GPIO     ((gpio_num_t)CONFIG_LUNAMON_STATUS_LED_GPIO)
#else
#define STATUS_LED_GPIO     (GPIO_NUM_0)
#endif

#if CONFIG_LUNAMON_I2C_ENABLED
#define I2C_MASTER_NUM      ((i2c_port_t)CONFIG_LUNAMON_I2C_MASTER_NUM)
#define I2C_MASTER_SCL_IO   ((gpio_num_t)CONFIG_LUNAMON_I2C_MASTER_SCL_IO)
#define I2C_MASTER_SDA_IO   ((gpio_num_t)CONFIG_LUNAMON_I2C_MASTER_SDA_IO)
#else
#define I2C_MASTER_NUM      (I2C_NUM_0)
#define I2C_MASTER_SCL_IO   (GPIO_NUM_0)
#define I2C_MASTER_SDA_IO   (GPIO_NUM_0)
#endif

LunaMon::LunaMon()
    : mqttBroker(wifiManager, dataModel), logger(LOGGER_LEVEL_DEBUG), ic2Master(nullptr),
      environmentalMon(nullptr) {
    logger.enableModuleDebug(LOGGER_MODULE_NMEA);
    logger.initForTask();

    if (CONFIG_LUNAMON_STATUS_LED_ENABLED) {
        statusLED = new StatusLED(STATUS_LED_GPIO);
        if (!statusLED) {
            logger << logErrorMain << "Failed to allocate status LED." << eol;
        }
    } else {
        statusLED = nullptr;
    }

    if (CONFIG_LUNAMON_NMEA_WIFI_ENABLED) {
        nmeaWiFiSource = new NMEAWiFiSource(wifiManager, CONFIG_LUNAMON_NMEA_WIFI_SOURCE_IPV4_ADDR,
                                            CONFIG_LUNAMON_NMEA_WIFI_SOURCE_TCP_PORT);
        if (!nmeaWiFiSource) {
            logger << logErrorMain << "Failed to allocate WiFi NMEA source." << eol;
        }
    } else {
        nmeaWiFiSource = nullptr;
    }

    if (CONFIG_LUNAMON_I2C_ENABLED) {
        ic2Master = new I2CMaster(I2C_MASTER_NUM, I2C_MASTER_SCL_IO, I2C_MASTER_SDA_IO);
    }

    if (CONFIG_LUNAMON_I2C_ENABLED &&
        (CONFIG_LUNAMON_BME280_ENABLED || CONFIG_LUNAMON_ENS160_ENABLED)) {
        environmentalMon = new EnvironmentalMon(dataModel, *ic2Master, statusLED);
    }
}

void LunaMon::run() {
    initNVS();

    dataModel.start();
    wifiManager.start();
    mqttBroker.start();

    if (nmeaWiFiSource) {
        nmeaWiFiSource->start();
    }

    if (environmentalMon) {
        environmentalMon->start();
    }

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}

void LunaMon::initNVS() {
    esp_err_t error;

    if ((error = nvs_flash_init()) != ESP_OK) {
        if (error == ESP_ERR_NVS_NO_FREE_PAGES || error == ESP_ERR_NVS_NEW_VERSION_FOUND) {
            logger << logNotifyMain << "Erasing flash" << eol;
            if ((error = nvs_flash_erase()) != ESP_OK) {
                logger << logNotifyMain << "Failed to erase flash: " << ESPError(error) << eol;
                errorExit();
            }

            if ((error = nvs_flash_init()) != ESP_OK) {
                logger << logNotifyMain << "Failed to initialize flash: " << ESPError(error) << eol;
                errorExit();
            }
        } else {
            logger << logNotifyMain << "Failed to initialize flash: " << ESPError(error) << eol;
            errorExit();
        }
    }
}
