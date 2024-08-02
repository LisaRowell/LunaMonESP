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
#include "DataModel.h"
#include "DataModelStringLeaf.h"
#include "DataModelUInt32Leaf.h"
#include "AISContacts.h"
#include "NMEA.h"
#include "NMEAWiFiSource.h"
#include "NMEAUARTSource.h"
#include "NMEADataModelBridge.h"
#include "LogManager.h"
#include "StatusLED.h"
#include "I2CMaster.h"
#include "EnvironmentalMon.h"
#include "Logger.h"
#include "ESPError.h"
#include "Error.h"

#include "driver/gpio.h"
#include "driver/i2c.h"
#include "driver/uart.h"
#include "nvs_flash.h"
#include "esp_timer.h"
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

#if CONFIG_LUNAMON_NMEA_UART1_ENABLED
#define NMEA_UART1_BAUD_RATE    (CONFIG_LUNAMON_NMEA_UART1_BAUD_RATE)
#define NMEA_UART1_RX_PIN       (CONFIG_LUNAMON_NMEA_UART1_RX_PIN)
#define NMEA_UART1_TX_PIN       (CONFIG_LUNAMON_NMEA_UART1_TX_PIN)
#else
#define NMEA_UART1_BAUD_RATE    (0)
#define NMEA_UART1_RX_PIN       (GPIO_NUM_0)
#define NMEA_UART1_TX_PIN       (GPIO_NUM_0)
#endif

#if CONFIG_LUNAMON_NMEA_UART2_ENABLED
#define NMEA_UART2_BAUD_RATE    (CONFIG_LUNAMON_NMEA_UART2_BAUD_RATE)
#define NMEA_UART2_RX_PIN       (CONFIG_LUNAMON_NMEA_UART2_RX_PIN)
#define NMEA_UART2_TX_PIN       (CONFIG_LUNAMON_NMEA_UART2_TX_PIN)
#else
#define NMEA_UART2_BAUD_RATE    (0)
#define NMEA_UART2_RX_PIN       (GPIO_NUM_0)
#define NMEA_UART2_TX_PIN       (GPIO_NUM_0)
#endif

LunaMon::LunaMon()
    : dataModel(statsManager),
      mqttBroker(wifiManager, dataModel, statsManager),
      nmea(dataModel),
      nmeaDataModelBridge(dataModel, statsManager),
      logManager(dataModel),
      logger(LOGGER_LEVEL_DEBUG),
      ic2Master(nullptr),
      environmentalMon(nullptr),
      versionLeaf("version", &dataModel.brokerNode(), versionBuffer),
      uptimeLeaf("uptime", &dataModel.brokerNode()) {
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
        nmeaWiFiSource = new NMEAWiFiSource(wifiManager, statsManager,
                                            CONFIG_LUNAMON_NMEA_WIFI_SOURCE_IPV4_ADDR,
                                            CONFIG_LUNAMON_NMEA_WIFI_SOURCE_TCP_PORT, nmea,
                                            aisContacts);
        if (nmeaWiFiSource) {
            nmeaWiFiSource->addMessageHandler(nmeaDataModelBridge);
        } else {
            logger << logErrorMain << "Failed to allocate WiFi NMEA source." << eol;
        }
    } else {
        nmeaWiFiSource = nullptr;
    }

    if (CONFIG_LUNAMON_NMEA_UART1_ENABLED) {
        nmeaUART1Source = new NMEAUARTSource("uart1", UART_NUM_1, NMEA_UART1_RX_PIN,
                                             NMEA_UART1_TX_PIN,NMEA_UART1_BAUD_RATE, statsManager,
                                             nmea, aisContacts);
        if (nmeaUART1Source) {
            nmeaUART1Source->addMessageHandler(nmeaDataModelBridge);
        } else {
            logger << logErrorMain << "Failed to allocate UART1 NMEA source." << eol;
        }
    } else {
        nmeaUART1Source = nullptr;
    }

    if (CONFIG_LUNAMON_NMEA_UART2_ENABLED) {
        nmeaUART2Source = new NMEAUARTSource("uart2", UART_NUM_2, NMEA_UART2_RX_PIN,
                                             NMEA_UART2_TX_PIN, NMEA_UART2_BAUD_RATE, statsManager,
                                             nmea, aisContacts);
        if (nmeaUART2Source) {
            nmeaUART2Source->addMessageHandler(nmeaDataModelBridge);
        } else {
            logger << logErrorMain << "Failed to allocate UART2 NMEA source." << eol;
        }
    } else {
        nmeaUART2Source = nullptr;
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

    statsManager.start();
    dataModel.start();
    aisContacts.start();
    wifiManager.start();
    mqttBroker.start();

    if (nmeaWiFiSource) {
        nmeaWiFiSource->start();
    }
    if (nmeaUART1Source) {
        nmeaUART1Source->start();
    }
    if (nmeaUART2Source) {
        nmeaUART2Source->start();
    }

    if (environmentalMon) {
        environmentalMon->start();
    }

    versionLeaf = "0.1.1";

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(10000));
        uptimeLeaf = (uint32_t)(esp_timer_get_time() / 1000000);
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
