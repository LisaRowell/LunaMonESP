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
#include "STALK.h"
#include "InterfaceProtocol.h"
#include "NMEAWiFiInterface.h"
#include "UARTInterface.h"
#include "NMEAUARTInterface.h"
#include "NMEADataModelBridge.h"
#include "STALKUARTInterface.h"
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

#if CONFIG_LUNAMON_UART1_ENABLED
#define UART1_BAUD_RATE         (CONFIG_LUNAMON_UART1_BAUD_RATE)
#define UART1_RX_PIN            (CONFIG_LUNAMON_UART1_RX_PIN)
#define UART1_TX_PIN            (CONFIG_LUNAMON_UART1_TX_PIN)
#ifdef CONFIG_LUNAMON_UART1_NMEA_0183
#define UART1_PROTOCOL          (INTERFACE_NMEA_O183)
#elif CONFIG_LUNAMON_UART1_STALK
#define UART1_PROTOCOL          (INTERFACE_STALK)
#else
Make sure and run menuconfig!
#endif
#else
#define UART1_BAUD_RATE         (0)
#define UART1_RX_PIN            (GPIO_NUM_0)
#define UART1_TX_PIN            (GPIO_NUM_0)
#define UART1_PROTOCOL          (INTERFACE_OFFLINE)
#endif

#if CONFIG_LUNAMON_UART2_ENABLED
#define UART2_BAUD_RATE         (CONFIG_LUNAMON_UART2_BAUD_RATE)
#define UART2_RX_PIN            (CONFIG_LUNAMON_UART2_RX_PIN)
#define UART2_TX_PIN            (CONFIG_LUNAMON_UART2_TX_PIN)
#ifdef CONFIG_LUNAMON_UART2_NMEA_0183
#define UART2_PROTOCOL          (INTERFACE_NMEA_O183)
#elif CONFIG_LUNAMON_UART2_STALK
#define UART2_PROTOCOL          (INTERFACE_STALK)
]#else
Make sure and run menuconfig!
#endif
#else
#define UART2_BAUD_RATE         (0)
#define UART2_RX_PIN            (GPIO_NUM_0)
#define UART2_TX_PIN            (GPIO_NUM_0)
#define UART2_PROTOCOL          (INTERFACE_OFFLINE)
#endif

LunaMon::LunaMon()
    : dataModel(statsManager),
      mqttBroker(wifiManager, dataModel, statsManager),
      nmea(dataModel),
      stalk(dataModel),
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
        nmeaWiFiInterface = new NMEAWiFiInterface("wifi", CONFIG_LUNAMON_NMEA_WIFI_SOURCE_IPV4_ADDR,
                                                  CONFIG_LUNAMON_NMEA_WIFI_SOURCE_TCP_PORT,
                                                  wifiManager, statsManager, nmea, aisContacts);
        if (nmeaWiFiInterface) {
            nmeaWiFiInterface->addMessageHandler(nmeaDataModelBridge);
        } else {
            logger << logErrorMain << "Failed to allocate WiFi NMEA interface." << eol;
        }
    } else {
        nmeaWiFiInterface = nullptr;
    }

    uart1Interface = createUARTInterface(UART1_PROTOCOL, "uart1", (uart_port_t)1, UART1_RX_PIN,
                                         UART1_TX_PIN, UART1_BAUD_RATE);
    uart2Interface = createUARTInterface(UART2_PROTOCOL, "uart2", (uart_port_t)2, UART2_RX_PIN,
                                         UART2_TX_PIN, UART2_BAUD_RATE);

    if (CONFIG_LUNAMON_I2C_ENABLED) {
        ic2Master = new I2CMaster(I2C_MASTER_NUM, I2C_MASTER_SCL_IO, I2C_MASTER_SDA_IO);
    }

    if (CONFIG_LUNAMON_I2C_ENABLED &&
        (CONFIG_LUNAMON_BME280_ENABLED || CONFIG_LUNAMON_ENS160_ENABLED)) {
        environmentalMon = new EnvironmentalMon(dataModel, *ic2Master, statusLED);
    }
}

UARTInterface *LunaMon::createUARTInterface(enum InterfaceProtocol protocol, const char *name,
                                            uart_port_t uartNumber, int rxPin, int txPin,
                                            int baudRate) {
    UARTInterface *uartInterface;

    switch (protocol) {
        case INTERFACE_NMEA_O183:
            uartInterface = createNMEAUARTInterface(name, uartNumber, rxPin, txPin, baudRate);
            break;

        case INTERFACE_STALK:
            uartInterface = createSTALKUARTInterface(name, uartNumber, rxPin, txPin, baudRate);
            break;

        case INTERFACE_OFFLINE:
        default:
            uartInterface = nullptr;
    }

    return uartInterface;
}

NMEAUARTInterface *LunaMon::createNMEAUARTInterface(const char *name, uart_port_t uartNumber,
                                                    int rxPin, int txPin, int baudRate) {
    NMEAUARTInterface *nmeaUARTInterface;

    nmeaUARTInterface = new NMEAUARTInterface(name, uartNumber, rxPin, txPin, baudRate,
                                              statsManager, nmea, aisContacts);
    if (nmeaUARTInterface) {
        nmeaUARTInterface->addMessageHandler(nmeaDataModelBridge);
    } else {
        logger << logErrorMain << "Failed to allocate " << name << " NMEA interface for UART "
                << uartNumber << "." << eol;
    }

    return nmeaUARTInterface;
}

STALKUARTInterface *LunaMon::createSTALKUARTInterface(const char *name, uart_port_t uartNumber,
                                                      int rxPin, int txPin, int baudRate) {
    STALKUARTInterface *stalkUARTInterface;

    stalkUARTInterface = new STALKUARTInterface(name, uartNumber, rxPin, txPin, baudRate,
                                                statsManager, stalk);
    if (!stalkUARTInterface) {
        logger << logErrorMain << "Failed to allocate " << name << " STALK interface for UART "
                << uartNumber << "." << eol;
    }

    return stalkUARTInterface;
}

void LunaMon::run() {
    initNVS();

    statsManager.start();
    dataModel.start();
    aisContacts.start();
    wifiManager.start();
    mqttBroker.start();

    if (nmeaWiFiInterface) {
        nmeaWiFiInterface->start();
    }
    if (uart1Interface) {
        uart1Interface->start();
    }
    if (uart2Interface) {
        uart2Interface->start();
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
