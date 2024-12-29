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
#include "InterfaceID.h"
#include "InterfaceProtocol.h"
#include "LogManager.h"
#include "Logger.h"

#include "etl/string.h"

#include "driver/uart.h"
#include "driver/gpio.h"

#include <stdint.h>

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
class NMEALineHandler;
class SeaTalkInterface;
class Interface;
class STALKUARTInterface;
class STALKRMTUARTInterface;
class SeaTalkRMTUARTInterface;
class NMEAServer;
class NMEABridge;
class SeaTalkNMEABridge;
class Buzzer;

class LunaMon {
    private:
        static constexpr uint32_t memoryCheckIntervalSec = 60;

        StatsManager statsManager;
        DataModel dataModel;
        AISContacts aisContacts;
        WiFiManager wifiManager;
        MQTTBroker mqttBroker;
        InstrumentData instrumentData;
        DataModelBridge dataModelBridge;
        LogManager logManager;
        Logger logger;
        NMEAServer *nmeaServer;
        NMEAWiFiInterface *nmeaWiFiInterface;
        UARTInterface *uart1Interface;
        UARTInterface *uart2Interface;
        SoftUARTInterface *softUARTInterface;
        RMTUARTInterface *rmtUARTInterface;
        NMEABridge *nmeaBridge;
        SeaTalkNMEABridge *seaTalkNMEABridge;
        I2CMaster *ic2Master;
        EnvironmentalMon *environmentalMon;
        StatusLED *statusLED;
        Buzzer *buzzer;
        PassiveTimer memoryCheckTimer;

        etl::string<10> versionBuffer;
        DataModelStringLeaf versionLeaf;
        DataModelUInt32Leaf uptimeLeaf;

        void initNVS();
        NMEAServer *createNMEAServer(uint16_t knownPort);
        UARTInterface *createUARTInterface(const char *name, const char *label,
                                           enum InterfaceProtocol protocol, uart_port_t uartNumber,
                                           gpio_num_t rxPin, gpio_num_t txPin, uint32_t baudRate,
                                           const char *filteredNMEATalkersList);
        NMEAUARTInterface *createNMEAUARTInterface(const char *name, const char *label,
                                                   uart_port_t uartNumber, gpio_num_t rxPin,
                                                   gpio_num_t txPin, uint32_t baudRate,
                                                   const char *filteredTalkersList);
        STALKUARTInterface *createSTALKUARTInterface(const char *name, const char *label,
                                                     uart_port_t uartNumber, gpio_num_t rxPin,
                                                     gpio_num_t txPin, uint32_t baudRate);
        SoftUARTInterface *createSoftUARTInterface(const char *name, const char *label,
                                                   enum InterfaceProtocol protocol,
                                                   gpio_num_t rxPin, gpio_num_t txPin,
                                                   const char *filteredNMEATalkersList);
        NMEASoftUARTInterface *createNMEASoftUARTInterface(const char *name, const char *label,
                                                           gpio_num_t rxPin, gpio_num_t txPin,
                                                           const char *filteredTalkersList);
        RMTUARTInterface *createRMTUARTInterface(const char *name, const char *label,
                                                 InterfaceProtocol protocol, gpio_num_t rxGPIO,
                                                 gpio_num_t txGPIO, uint32_t baudRate,
                                                 const char *filteredNMEATalkersList);
        NMEARMTUARTInterface *createNMEARMTUARTInterface(const char *name, const char *label,
                                                         gpio_num_t rxGPIO, gpio_num_t txGPIO,
                                                         uint32_t baudRate,
                                                         const char *filteredTallkersList);
        STALKRMTUARTInterface *createSTALKRMTUARTInterface(const char *name, const char *label,
                                                           gpio_num_t rxGPIO, gpio_num_t txGPIO,
                                                           uint32_t baudRate);
        SeaTalkRMTUARTInterface *createSeaTalkRMTUARTInterface(const char *name, const char *label,
                                                               gpio_num_t rxPin, gpio_num_t txPin);
        NMEABridge *createNMEABridge(const char *name, const char *msgTypeList,
                                     InterfaceID srcInterfaceID, InterfaceID dstInterfaceID);
        SeaTalkNMEABridge *createSeaTalkNMEABridge(const char *name, const char *label,
                                                   InterfaceID srcInterfaceID,
                                                   InterfaceID dstInterfaceID,
                                                   const char *talkerCode);
        NMEAInterface *nmeaInterfaceByID(InterfaceID id);
        NMEALineHandler *nmeaDestinationByID(InterfaceID id);
        SeaTalkInterface *seaTalkInterfaceByID(InterfaceID id);
        Interface *interfaceByID(InterfaceID id);
        void checkMemoryUsage();
        void logMainStackSize();

    public:
        LunaMon();
        void run();
};

#endif // LUNA_MON_H
