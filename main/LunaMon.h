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

#ifndef LUNA_MON_H
#define LUNA_MON_H

#include "StatsManager.h"

#include "DataModel.h"
#include "DataModelStringLeaf.h"
#include "DataModelUInt32Leaf.h"

#include "AISContacts.h"

#include "WiFiManager.h"
#include "MQTTBroker.h"
#include "InstrumentData.h"
#include "DataModelBridge.h"
#include "InterfaceProtocol.h"
#include "LogManager.h"
#include "Logger.h"

#include "etl/string.h"

#include "driver/uart.h"
#include "driver/gpio.h"

class StatusLED;
class I2CMaster;
class EnvironmentalMon;
class UARTInterface;
class SoftUARTInterface;
class NMEASoftUARTInterface;
class RMTUARTInterface;
class NMEARMTUARTInterface;
class NMEAWiFiInterface;
class NMEAUARTInterface;
class NMEAInterface;
class Interface;
class STALKUARTInterface;
class NMEABridge;

class LunaMon {
    private:
        enum InterfaceID : uint8_t {
            INTERFACE_WIFI,
            INTERFACE_UART1,
            INTERFACE_UART2,
            INTERFACE_SOFT_UART,
            INTERFACE_NONE
        };

        StatsManager statsManager;
        DataModel dataModel;
        AISContacts aisContacts;
        WiFiManager wifiManager;
        MQTTBroker mqttBroker;
        InstrumentData instrumentData;
        DataModelBridge dataModelBridge;
        LogManager logManager;
        Logger logger;
        NMEAWiFiInterface *nmeaWiFiInterface;
        UARTInterface *uart1Interface;
        UARTInterface *uart2Interface;
        SoftUARTInterface *softUARTInterface;
        RMTUARTInterface *rmtUARTInterface;
        NMEABridge *nmeaBridge;
        I2CMaster *ic2Master;
        EnvironmentalMon *environmentalMon;
        StatusLED *statusLED;

        etl::string<10> versionBuffer;
        DataModelStringLeaf versionLeaf;
        DataModelUInt32Leaf uptimeLeaf;

        void initNVS();
        UARTInterface *createUARTInterface(enum InterfaceProtocol protocol, const char *name,
                                           uart_port_t uartNumber, int rxPin, int txPin,
                                           int baudRate);
        NMEAUARTInterface *createNMEAUARTInterface(const char *name, uart_port_t uartNumber,
                                                   int rxPin, int txPin, int baudRate);
        STALKUARTInterface *createSTALKUARTInterface(const char *name, uart_port_t uartNumber,
                                                     int rxPin, int txPin, int baudRate);
        SoftUARTInterface *createSoftUARTInterface(enum InterfaceProtocol protocol,
                                                   const char *name, gpio_num_t rxPin,
                                                   gpio_num_t txPin);
        NMEASoftUARTInterface *createNMEASoftUARTInterface(const char *name, gpio_num_t rxPin,
                                                           gpio_num_t txPin);
        RMTUARTInterface *createRMTUARTInterface(InterfaceProtocol protocol, const char *name,
                                                 gpio_num_t rxGPIO, gpio_num_t txGPIO);
        NMEARMTUARTInterface *createNMEARMTUARTInterface(const char *name, gpio_num_t rxGPIO,
                                                         gpio_num_t txGPIO);
        NMEABridge *createNMEABridge(const char *name, const char *msgTypeList,
                                     InterfaceID srcInterfaceID, InterfaceID dstInterfaceID);
        NMEAInterface *nmeaInterfaceByID(InterfaceID id);
        Interface *interfaceByID(InterfaceID id);

    public:
        LunaMon();
        void run();
};

#endif // LUNA_MON_H
