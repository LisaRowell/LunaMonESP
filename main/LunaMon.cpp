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
#include "InterfaceID.h"
#include "InterfaceProtocol.h"
#include "NMEAWiFiInterface.h"
#include "UARTInterface.h"
#include "NMEAUARTInterface.h"
#include "InstrumentData.h"
#include "DataModelBridge.h"
#include "STALKUARTInterface.h"
#include "SoftUARTInterface.h"
#include "NMEASoftUARTInterface.h"
#include "RMTUARTInterface.h"
#include "NMEARMTUARTInterface.h"
#include "STALKRMTUARTInterface.h"
#include "NMEABridge.h"
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

#include "driver/gpio.h"
#include "driver/uart.h"

#define STATUS_LED_GPIO     ((gpio_num_t)CONFIG_LUNAMON_STATUS_LED_GPIO)

#define I2C_MASTER_NUM      ((i2c_port_t)CONFIG_LUNAMON_I2C_MASTER_NUM)
#define I2C_MASTER_SCL_IO   ((gpio_num_t)CONFIG_LUNAMON_I2C_MASTER_SCL_IO)
#define I2C_MASTER_SDA_IO   ((gpio_num_t)CONFIG_LUNAMON_I2C_MASTER_SDA_IO)

#define NMEA_WIFI_SOURCE_LABEL      (CONFIG_LUNAMON_NMEA_WIFI_SOURCE_LABEL)
#define NMEA_WIFI_SOURCE_IPV4_ADDR  (CONFIG_LUNAMON_NMEA_WIFI_SOURCE_IPV4_ADDR)
#define NMEA_WIFI_SOURCE_TCP_PORT   (CONFIG_LUNAMON_NMEA_WIFI_SOURCE_TCP_PORT)

#ifdef CONFIG_LUNAMON_UART1_NMEA_0183
#define UART1_PROTOCOL          (INTERFACE_NMEA_O183)
#elif CONFIG_LUNAMON_UART1_STALK
#define UART1_PROTOCOL          (INTERFACE_STALK)
#elif CONFIG_LUNAMON_UART1_OFFLINE
#define UART1_PROTOCOL          (INTERFACE_OFFLINE)
#else
Make sure and run menuconfig!
#endif
#define UART1_LABEL             (CONFIG_LUNAMON_UART1_LABEL)
#define UART1_BAUD_RATE         (CONFIG_LUNAMON_UART1_BAUD_RATE)
#define UART1_RX_PIN            ((gpio_num_t)CONFIG_LUNAMON_UART1_RX_PIN)
#define UART1_TX_PIN            ((gpio_num_t)CONFIG_LUNAMON_UART1_TX_PIN)

#ifdef CONFIG_LUNAMON_UART2_NMEA_0183
#define UART2_PROTOCOL          (INTERFACE_NMEA_O183)
#elif CONFIG_LUNAMON_UART2_STALK
#define UART2_PROTOCOL          (INTERFACE_STALK)
#elif CONFIG_LUNAMON_UART2_OFFLINE
#define UART2_PROTOCOL          (INTERFACE_OFFLINE)
#else
Make sure and run menuconfig!
#endif
#define UART2_LABEL             (CONFIG_LUNAMON_UART2_LABEL)
#define UART2_BAUD_RATE         (CONFIG_LUNAMON_UART2_BAUD_RATE)
#define UART2_RX_PIN            ((gpio_num_t)CONFIG_LUNAMON_UART2_RX_PIN)
#define UART2_TX_PIN            ((gpio_num_t)CONFIG_LUNAMON_UART2_TX_PIN)

#ifdef CONFIG_LUNAMON_SOFTWARE_UART_NMEA_0183
#define SOFTWARE_UART_PROTOCOL  (INTERFACE_NMEA_O183)
#elif CONFIG_LUNAMON_SOFTWARE_UART_OFFLINE
#define SOFTWARE_UART_PROTOCOL  (INTERFACE_OFFLINE)
#else
Make sure and run menuconfig!
#endif
#define SOFTWARE_UART_LABEL     (CONFIG_LUNAMON_SOFTWARE_UART_LABEL)
#define SOFTWARE_UART_RX_PIN    ((gpio_num_t)CONFIG_LUNAMON_SOFTWARE_UART_RX_PIN)
#define SOFTWARE_UART_TX_PIN    ((gpio_num_t)CONFIG_LUNAMON_SOFTWARE_UART_TX_PIN)

#if CONFIG_LUNAMON_RMT_UART_NMEA_0183
#define RMT_UART_PROTOCOL       (INTERFACE_NMEA_O183)
#elif CONFIG_LUNAMON_RMT_UART_STALK
#define RMT_UART_PROTOCOL       (INTERFACE_STALK)
#elif CONFIG_LUNAMON_RMT_UART_SEA_TALK
#define RMT_UART_PROTOCOL       (INTERFACE_SEA_TALK)
#elif CONFIG_LUNAMON_RMT_UART_OFFLINE
#define RMT_UART_PROTOCOL       (INTERFACE_OFFLINE)
#else
Make sure and run menuconfig!
#endif
#define RMT_UART_LABEL          (CONFIG_LUNAMON_RMT_UART_LABEL)
#define RMT_UART_RX_GPIO        ((gpio_num_t)CONFIG_LUNAMON_RMT_UART_RX_GPIO)
#define RMT_UART_TX_GPIO        ((gpio_num_t)CONFIG_LUNAMON_RMT_UART_TX_GPIO)
#define RMT_UART_BAUD_RATE      ((uint32_t)CONFIG_LUNAMON_RMT_UART_BAUD_RATE)

#define NMEA_BRIDGE_NAME            (CONFIG_LUNAMON_NMEA_BRIDGE_NAME)
#if CONFIG_LUNAMON_NMEA_BRIDGE_SOURCE_WIFI
#define NMEA_BRIDGE_SOURCE          (InterfaceID::INTERFACE_WIFI)
#elif CONFIG_LUNAMON_NMEA_BRIDGE_SOURCE_UART1
#define NMEA_BRIDGE_SOURCE          (InterfaceID::INTERFACE_UART1)
#elif CONFIG_LUNAMON_NMEA_BRIDGE_SOURCE_UART2
#define NMEA_BRIDGE_SOURCE          (InterfaceID::INTERFACE_UART2)
#elif CONFIG_LUNAMON_NMEA_BRIDGE_SOURCE_RMT_UART
#define NMEA_BRIDGE_SOURCE          (InterfaceID::INTERFACE_RMT_UART)
#elif CONFIG_LUNAMON_NMEA_BRIDGE_SOURCE_SOFT_UART
#define NMEA_BRIDGE_SOURCE          (InterfaceID::INTERFACE_SOFT_UART)
#elif CONFIG_LUNAMON_NMEA_BRIDGE_SOURCE_NONE
#define NMEA_BRIDGE_SOURCE          (InterfaceID::INTERFACE_NONE)
#else
Make sure and run menuconfig!
#endif
#if CONFIG_LUNAMON_NMEA_BRIDGE_DESTINATION_WIFI
#define NMEA_BRIDGE_DESTINATION     (InterfaceID::INTERFACE_WIFI)
#elif CONFIG_LUNAMON_NMEA_BRIDGE_DESTINATION_UART1
#define NMEA_BRIDGE_DESTINATION     (InterfaceID::INTERFACE_UART1)
#elif CONFIG_LUNAMON_NMEA_BRIDGE_DESTINATION_UART2
#define NMEA_BRIDGE_DESTINATION     (InterfaceID::INTERFACE_UART2)
#elif CONFIG_LUNAMON_NMEA_BRIDGE_DESTINATION_RMT_UART
#define NMEA_BRIDGE_DESTINATION     (InterfaceID::INTERFACE_RMT_UART)
#elif CONFIG_LUNAMON_NMEA_BRIDGE_DESTINATION_SOFT_UART
#define NMEA_BRIDGE_DESTINATION     (InterfaceID::INTERFACE_SOFT_UART)
#elif CONFIG_LUNAMON_NMEA_BRIDGE_DESTINATION_NONE
#define NMEA_BRIDGE_DESTINATION     (InterfaceID::INTERFACE_NONE)
#else
Make sure and run menuconfig!
#endif
#define NMEA_BRIDGE_MESSAGE_TYPES   (CONFIG_LUNAMON_NMEA_BRIDGE_MESSAGE_TYPES)

LunaMon::LunaMon()
    : dataModel(statsManager),
      mqttBroker(wifiManager, dataModel, statsManager),
      instrumentData(dataModel, statsManager),
      dataModelBridge(instrumentData),
      logManager(dataModel),
      logger(LOGGER_LEVEL_DEBUG),
      nmeaBridge(nullptr),
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
        nmeaWiFiInterface = new NMEAWiFiInterface("wifi", NMEA_WIFI_SOURCE_LABEL,
                                                  NMEA_WIFI_SOURCE_IPV4_ADDR,
                                                  NMEA_WIFI_SOURCE_TCP_PORT, wifiManager,
                                                  statsManager, aisContacts, dataModel);
        if (nmeaWiFiInterface) {
            nmeaWiFiInterface->addMessageHandler(dataModelBridge);
        } else {
            logger << logErrorMain << "Failed to allocate WiFi NMEA interface." << eol;
        }
    } else {
        nmeaWiFiInterface = nullptr;
    }

    uart1Interface = createUARTInterface("uart1", UART1_LABEL, UART1_PROTOCOL, UART_NUM_1,
                                         UART1_RX_PIN, UART1_TX_PIN, UART1_BAUD_RATE);
    uart2Interface = createUARTInterface("uart2", UART2_LABEL, UART2_PROTOCOL, UART_NUM_2,
                                         UART2_RX_PIN, UART2_TX_PIN, UART2_BAUD_RATE);
    softUARTInterface = createSoftUARTInterface("softUART", SOFTWARE_UART_LABEL,
                                                SOFTWARE_UART_PROTOCOL, SOFTWARE_UART_RX_PIN,
                                                SOFTWARE_UART_TX_PIN);
    rmtUARTInterface = createRMTUARTInterface("rmtUART", RMT_UART_LABEL, RMT_UART_PROTOCOL,
                                              RMT_UART_RX_GPIO, RMT_UART_TX_GPIO,
                                              RMT_UART_BAUD_RATE);

    if (CONFIG_LUNAMON_NMEA_BRIDGE_ENABLED) {
        nmeaBridge = createNMEABridge(NMEA_BRIDGE_NAME, NMEA_BRIDGE_MESSAGE_TYPES,
                                      NMEA_BRIDGE_SOURCE, NMEA_BRIDGE_DESTINATION);
    }

    if (CONFIG_LUNAMON_I2C_ENABLED) {
        ic2Master = new I2CMaster(I2C_MASTER_NUM, I2C_MASTER_SCL_IO, I2C_MASTER_SDA_IO);
    }

    if (CONFIG_LUNAMON_I2C_ENABLED &&
        (CONFIG_LUNAMON_BME280_ENABLED || CONFIG_LUNAMON_ENS160_ENABLED)) {
        environmentalMon = new EnvironmentalMon(dataModel, *ic2Master, statusLED);
    }
}

UARTInterface *LunaMon::createUARTInterface(const char *name, const char *label,
                                            enum InterfaceProtocol protocol, uart_port_t uartNumber,
                                            gpio_num_t rxPin, gpio_num_t txPin, uint32_t baudRate) {
    UARTInterface *uartInterface;

    switch (protocol) {
        case INTERFACE_NMEA_O183:
            uartInterface = createNMEAUARTInterface(name, label, uartNumber, rxPin, txPin,
                                                    baudRate);
            break;

        case INTERFACE_STALK:
            uartInterface = createSTALKUARTInterface(name, label, uartNumber, rxPin, txPin,
                                                     baudRate);
            break;

        case INTERFACE_OFFLINE:
        default:
            uartInterface = nullptr;
    }

    return uartInterface;
}

NMEAUARTInterface *LunaMon::createNMEAUARTInterface(const char *name, const char *label,
                                                    uart_port_t uartNumber, gpio_num_t rxPin,
                                                    gpio_num_t txPin, uint32_t baudRate) {
    NMEAUARTInterface *nmeaUARTInterface;

    nmeaUARTInterface = new NMEAUARTInterface(name, label, uartNumber, rxPin, txPin, baudRate,
                                              statsManager, aisContacts, dataModel);
    if (nmeaUARTInterface) {
        nmeaUARTInterface->addMessageHandler(dataModelBridge);
    } else {
        logger << logErrorMain << "Failed to allocate " << name << " NMEA interface for UART "
                << uartNumber << "." << eol;
    }

    return nmeaUARTInterface;
}

STALKUARTInterface *LunaMon::createSTALKUARTInterface(const char *name, const char *label,
                                                      uart_port_t uartNumber, gpio_num_t rxPin,
                                                      gpio_num_t txPin, uint32_t baudRate) {
    STALKUARTInterface *stalkUARTInterface;

    stalkUARTInterface = new STALKUARTInterface(name, label, uartNumber, rxPin, txPin, baudRate,
                                                instrumentData, statsManager, dataModel);
    if (!stalkUARTInterface) {
        logger << logErrorMain << "Failed to allocate " << name << " STALK interface for UART "
                << uartNumber << "." << eol;
    }

    return stalkUARTInterface;
}

SoftUARTInterface *LunaMon::createSoftUARTInterface(const char *name, const char *label,
                                                    enum InterfaceProtocol protocol,
                                                    gpio_num_t rxPin, gpio_num_t txPin) {
    SoftUARTInterface *softUARTInterface;

    switch (protocol) {
        case INTERFACE_NMEA_O183:
            softUARTInterface = createNMEASoftUARTInterface(name, label, rxPin, txPin);
            break;

        case INTERFACE_OFFLINE:
        default:
            softUARTInterface = nullptr;
    }

    return softUARTInterface;
}

NMEASoftUARTInterface *LunaMon::createNMEASoftUARTInterface(const char *name, const char *label,
                                                            gpio_num_t rxPin, gpio_num_t txPin) {
    NMEASoftUARTInterface *nmeaSoftUARTInterface;

    nmeaSoftUARTInterface = new NMEASoftUARTInterface(name, label, rxPin, txPin, 4800, statsManager,
                                                      aisContacts, dataModel);
    if (nmeaSoftUARTInterface) {
        nmeaSoftUARTInterface->addMessageHandler(dataModelBridge);
    } else {
        logger << logErrorMain << "Failed to allocate " << name
               << " NMEA interface for Software UART" << "." << eol;
    }

    return nmeaSoftUARTInterface;
}

RMTUARTInterface *LunaMon::createRMTUARTInterface(const char *name, const char *label,
                                                  InterfaceProtocol protocol, gpio_num_t rxGPIO,
                                                  gpio_num_t txGPIO, uint32_t baudRate) {
    RMTUARTInterface *rmtUARTInterface;

    switch (protocol) {
        case INTERFACE_NMEA_O183:
            rmtUARTInterface = createNMEARMTUARTInterface(name, label, rxGPIO, txGPIO, baudRate);
            break;

        case INTERFACE_STALK:
            rmtUARTInterface = createSTALKRMTUARTInterface(name, label, rxGPIO, txGPIO, baudRate);
            break;

        case INTERFACE_OFFLINE:
        default:
            rmtUARTInterface = nullptr;
    }

    return rmtUARTInterface;
}

NMEARMTUARTInterface *LunaMon::createNMEARMTUARTInterface(const char *name, const char *label,
                                                          gpio_num_t rxGPIO, gpio_num_t txGPIO,
                                                          uint32_t baudRate) {
    NMEARMTUARTInterface *nmeaRMTUARTInterface;

    nmeaRMTUARTInterface = new NMEARMTUARTInterface(name, label, rxGPIO, txGPIO, baudRate,
                                                    statsManager, aisContacts, dataModel);
    if (nmeaRMTUARTInterface) {
        nmeaRMTUARTInterface->addMessageHandler(dataModelBridge);
    } else {
        logger << logErrorMain << "Failed to allocate NMEA RMT UART interface " << name << eol;
    }

    return nmeaRMTUARTInterface;
}

STALKRMTUARTInterface *LunaMon::createSTALKRMTUARTInterface(const char *name, const char *label,
                                                            gpio_num_t rxPin, gpio_num_t txPin,
                                                            uint32_t baudRate) {
    STALKRMTUARTInterface *stalkRMTUARTInterface;

    stalkRMTUARTInterface = new STALKRMTUARTInterface(name, label, rxPin, txPin, baudRate,
                                                      instrumentData, statsManager, dataModel);
    if (!stalkRMTUARTInterface) {
        logger << logErrorMain << "Failed to allocate " << name << " STALK interface for RMT UART"
               << eol;
    }

    return stalkRMTUARTInterface;
}

NMEABridge *LunaMon::createNMEABridge(const char *name, const char *msgTypeList,
                                      InterfaceID srcInterfaceID, InterfaceID dstInterfaceID) {
    NMEAInterface *srcInterface = nmeaInterfaceByID(srcInterfaceID);
    if (srcInterface == nullptr) {
        logger << logErrorMain << "NMEA to NMEA Bridge '" << name
               << "' must have a NMEA configured source interface" << eol;
        return nullptr;
    }

    Interface *dstInterface = interfaceByID(dstInterfaceID);
    if (dstInterface == nullptr) {
        logger << logErrorMain << "NMEA to NMEA Bridge '" << name
               << "' must have a destination interface" << eol;
        return nullptr;
    }

    return new NMEABridge(name, msgTypeList, *srcInterface, *dstInterface, statsManager, dataModel);
}

NMEAInterface *LunaMon::nmeaInterfaceByID(InterfaceID id) {
    switch (id) {
        case InterfaceID::INTERFACE_WIFI:
            return nmeaWiFiInterface;
        case InterfaceID::INTERFACE_UART1:
            if (UART1_PROTOCOL == INTERFACE_NMEA_O183) {
                return (NMEAInterface *)uart1Interface;
            } else {
                return nullptr;
            }
        case InterfaceID::INTERFACE_UART2:
            if (UART2_PROTOCOL == INTERFACE_NMEA_O183) {
                return (NMEAInterface *)uart2Interface;
            } else {
                return nullptr;
            }
        case InterfaceID::INTERFACE_RMT_UART:
            if (RMT_UART_PROTOCOL == INTERFACE_NMEA_O183) {
                return (NMEAInterface *)rmtUARTInterface;
            } else {
                return nullptr;
            }
        case InterfaceID::INTERFACE_SOFT_UART:
            if (SOFTWARE_UART_PROTOCOL == INTERFACE_NMEA_O183) {
                return (NMEAInterface *)softUARTInterface;
            } else {
                return nullptr;
            }
        default:
            return nullptr;
    }
}

Interface *LunaMon::interfaceByID(InterfaceID id) {
    switch (id) {
        case InterfaceID::INTERFACE_WIFI:
            return nmeaWiFiInterface;
        case InterfaceID::INTERFACE_UART1:
            return uart1Interface;
        case InterfaceID::INTERFACE_UART2:
            return uart2Interface;
        case InterfaceID::INTERFACE_SOFT_UART:
            return softUARTInterface;
        default:
            return nullptr;
    }
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
    if (softUARTInterface) {
        softUARTInterface->start();
    }
    if (rmtUARTInterface) {
        rmtUARTInterface->start();
    }

    if (nmeaBridge) {
        nmeaBridge->start();
    }

    if (environmentalMon) {
        environmentalMon->start();
    }

    gpio_dump_io_configuration(stdout, SOC_GPIO_VALID_GPIO_MASK);

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
