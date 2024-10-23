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

#ifndef LOGGER_H
#define LOGGER_H

#include "etl/string.h"
#include "etl/string_view.h"

#include <esp_netif.h>

#include <stdint.h>

class LoggableItem;

enum LoggerModule {
    LOGGER_MODULE_MAIN,
    LOGGER_MODULE_DATA_MODEL,
    LOGGER_MODULE_MQTT,
    LOGGER_MODULE_NMEA,
    LOGGER_MODULE_DATA_MODEL_BRIDGE,
    LOGGER_MODULE_NMEA_WIFI,
    LOGGER_MODULE_NMEA_UART,
    LOGGER_MODULE_NMEA_SOFT_UART,
    LOGGER_MODULE_NMEA_RMT_UART,
    LOGGER_MODULE_STALK,
    LOGGER_MODULE_STALK_UART,
    LOGGER_MODULE_STALK_RMT_UART,
    LOGGER_MODULE_SEA_TALK,
    LOGGER_MODULE_SEA_TALK_RMT_UART,
    LOGGER_MODULE_NMEA_SERVER,
    LOGGER_MODULE_NMEA_BRIDGE,
    LOGGER_MODULE_NMEA_LINE,
    LOGGER_MODULE_SEA_TALK_NMEA_BRIDGE,
    LOGGER_MODULE_UART,
    LOGGER_MODULE_SOFT_UART,
    LOGGER_MODULE_AIS,
    LOGGER_MODULE_RMT_UART,
    LOGGER_MODULE_WIFI_INTERFACE,
    LOGGER_MODULE_WIFI_MANAGER,
    LOGGER_MODULE_STATS_MANAGER,
    LOGGER_MODULE_I2C_MASTER,
    LOGGER_MODULE_BME280_DRIVER,
    LOGGER_MODULE_ENS160_DRIVER,
    LOGGER_MODULE_ENVIRONMENTAL_MON,
    LOGGER_MODULE_TASK_OBJECT,
    LOGGER_MODULE_COUNT
};

enum LoggerLevel {
    LOGGER_LEVEL_DEBUG,
    LOGGER_LEVEL_WARNING,
    LOGGER_LEVEL_NOTIFY,
    LOGGER_LEVEL_ERROR
};
#define LOG_LEVEL_SHIFT 8
#define LOGGER_MODULE_MASK 0xff

#define LOG_SELECTOR(level, module) (((level) << LOG_LEVEL_SHIFT) | (module))
enum LogSelector {
    logDebugMain = LOG_SELECTOR(LOGGER_LEVEL_DEBUG, LOGGER_MODULE_MAIN),
    logDebugDataModel = LOG_SELECTOR(LOGGER_LEVEL_DEBUG, LOGGER_MODULE_DATA_MODEL),
    logDebugMQTT = LOG_SELECTOR(LOGGER_LEVEL_DEBUG, LOGGER_MODULE_MQTT),
    logDebugNMEA = LOG_SELECTOR(LOGGER_LEVEL_DEBUG, LOGGER_MODULE_NMEA),
    logDebugDataModelBridge = LOG_SELECTOR(LOGGER_LEVEL_DEBUG, LOGGER_MODULE_DATA_MODEL_BRIDGE),
    logDebugNMEAWiFi = LOG_SELECTOR(LOGGER_LEVEL_DEBUG, LOGGER_MODULE_NMEA_WIFI),
    logDebugNMEAUART = LOG_SELECTOR(LOGGER_LEVEL_DEBUG, LOGGER_MODULE_NMEA_UART),
    logDebugNMEASoftUART = LOG_SELECTOR(LOGGER_LEVEL_DEBUG, LOGGER_MODULE_NMEA_SOFT_UART),
    logDebugNMEARMTUART = LOG_SELECTOR(LOGGER_LEVEL_DEBUG, LOGGER_MODULE_NMEA_RMT_UART),
    logDebugSTALK = LOG_SELECTOR(LOGGER_LEVEL_DEBUG, LOGGER_MODULE_STALK),
    logDebugSTALKUART = LOG_SELECTOR(LOGGER_LEVEL_DEBUG, LOGGER_MODULE_STALK_UART),
    logDebugSTALKRMTUART = LOG_SELECTOR(LOGGER_LEVEL_DEBUG, LOGGER_MODULE_STALK_RMT_UART),
    logDebugSeaTalk = LOG_SELECTOR(LOGGER_LEVEL_DEBUG, LOGGER_MODULE_SEA_TALK),
    logDebugSeaTalkRMTUART = LOG_SELECTOR(LOGGER_LEVEL_DEBUG, LOGGER_MODULE_SEA_TALK_RMT_UART),
    logDebugNMEAServer = LOG_SELECTOR(LOGGER_LEVEL_DEBUG, LOGGER_MODULE_NMEA_SERVER),
    logDebugNMEABridge = LOG_SELECTOR(LOGGER_LEVEL_DEBUG, LOGGER_MODULE_NMEA_BRIDGE),
    logDebugNMEALine = LOG_SELECTOR(LOGGER_LEVEL_DEBUG, LOGGER_MODULE_NMEA_LINE),
    logDebugSeaTalkNMEABridge = LOG_SELECTOR(LOGGER_LEVEL_DEBUG,
                                             LOGGER_MODULE_SEA_TALK_NMEA_BRIDGE),
    logDebugUART = LOG_SELECTOR(LOGGER_LEVEL_DEBUG, LOGGER_MODULE_UART),
    logDebugSoftUART = LOG_SELECTOR(LOGGER_LEVEL_DEBUG, LOGGER_MODULE_SOFT_UART),
    logDebugAIS = LOG_SELECTOR(LOGGER_LEVEL_DEBUG, LOGGER_MODULE_AIS),
    logDebugRMTUART = LOG_SELECTOR(LOGGER_LEVEL_DEBUG, LOGGER_MODULE_RMT_UART),
    logDebugWiFiInterface = LOG_SELECTOR(LOGGER_LEVEL_DEBUG, LOGGER_MODULE_WIFI_INTERFACE),
    logDebugWiFiManager = LOG_SELECTOR(LOGGER_LEVEL_DEBUG, LOGGER_MODULE_WIFI_MANAGER),
    logDebugStatsManager = LOG_SELECTOR(LOGGER_LEVEL_DEBUG, LOGGER_MODULE_STATS_MANAGER),
    logDebugI2CMaster = LOG_SELECTOR(LOGGER_LEVEL_DEBUG, LOGGER_MODULE_I2C_MASTER),
    logDebugBME280Driver = LOG_SELECTOR(LOGGER_LEVEL_DEBUG, LOGGER_MODULE_BME280_DRIVER),
    logDebugENS160Driver = LOG_SELECTOR(LOGGER_LEVEL_DEBUG, LOGGER_MODULE_ENS160_DRIVER),
    logDebugEnvironmentalMon = LOG_SELECTOR(LOGGER_LEVEL_DEBUG, LOGGER_MODULE_ENVIRONMENTAL_MON),
    logDebugTaskObject = LOG_SELECTOR(LOGGER_LEVEL_DEBUG, LOGGER_MODULE_TASK_OBJECT),

    logWarnMain = LOG_SELECTOR(LOGGER_LEVEL_WARNING, LOGGER_MODULE_MAIN),
    logWarnDataModel = LOG_SELECTOR(LOGGER_LEVEL_WARNING, LOGGER_MODULE_DATA_MODEL),
    logWarnMQTT = LOG_SELECTOR(LOGGER_LEVEL_WARNING, LOGGER_MODULE_MQTT),
    logWarnNMEA = LOG_SELECTOR(LOGGER_LEVEL_WARNING, LOGGER_MODULE_NMEA),
    logWarnDataModelBridge = LOG_SELECTOR(LOGGER_LEVEL_WARNING, LOGGER_MODULE_DATA_MODEL_BRIDGE),
    logWarnNMEAWiFi = LOG_SELECTOR(LOGGER_LEVEL_WARNING, LOGGER_MODULE_NMEA_WIFI),
    logWarnNMEAUART = LOG_SELECTOR(LOGGER_LEVEL_WARNING, LOGGER_MODULE_NMEA_UART),
    logWarnNMEASoftUART = LOG_SELECTOR(LOGGER_LEVEL_WARNING, LOGGER_MODULE_NMEA_SOFT_UART),
    logWarnNMEARMTUART = LOG_SELECTOR(LOGGER_LEVEL_WARNING, LOGGER_MODULE_NMEA_RMT_UART),
    logWarnSTALK = LOG_SELECTOR(LOGGER_LEVEL_WARNING, LOGGER_MODULE_STALK),
    logWarnSTALKUART = LOG_SELECTOR(LOGGER_LEVEL_WARNING, LOGGER_MODULE_STALK_UART),
    logWarnSTALKRMTUART = LOG_SELECTOR(LOGGER_LEVEL_WARNING, LOGGER_MODULE_STALK_RMT_UART),
    logWarnSeaTalk = LOG_SELECTOR(LOGGER_LEVEL_WARNING, LOGGER_MODULE_SEA_TALK),
    logWarnSeaTalkRMTUART = LOG_SELECTOR(LOGGER_LEVEL_WARNING, LOGGER_MODULE_SEA_TALK_RMT_UART),
    logWarnNMEAServer = LOG_SELECTOR(LOGGER_LEVEL_WARNING, LOGGER_MODULE_NMEA_SERVER),
    logWarnNMEABridge = LOG_SELECTOR(LOGGER_LEVEL_WARNING, LOGGER_MODULE_NMEA_BRIDGE),
    logWarnNMEALine = LOG_SELECTOR(LOGGER_LEVEL_WARNING, LOGGER_MODULE_NMEA_LINE),
    logWarnSeaTalkNMEABridge = LOG_SELECTOR(LOGGER_LEVEL_WARNING,
                                            LOGGER_MODULE_SEA_TALK_NMEA_BRIDGE),
    logWarnUART = LOG_SELECTOR(LOGGER_LEVEL_WARNING, LOGGER_MODULE_UART),
    logWarnSoftUART = LOG_SELECTOR(LOGGER_LEVEL_WARNING, LOGGER_MODULE_SOFT_UART),
    logWarnAIS = LOG_SELECTOR(LOGGER_LEVEL_WARNING, LOGGER_MODULE_AIS),
    logWarnRMTUART = LOG_SELECTOR(LOGGER_LEVEL_WARNING, LOGGER_MODULE_RMT_UART),
    logWarnWiFiInterface = LOG_SELECTOR(LOGGER_LEVEL_WARNING, LOGGER_MODULE_WIFI_INTERFACE),
    logWarnWiFiManager = LOG_SELECTOR(LOGGER_LEVEL_WARNING, LOGGER_MODULE_WIFI_MANAGER),
    logWarnStatsManager = LOG_SELECTOR(LOGGER_LEVEL_WARNING, LOGGER_MODULE_STATS_MANAGER),
    logWarnI2CMaster = LOG_SELECTOR(LOGGER_LEVEL_WARNING, LOGGER_MODULE_I2C_MASTER),
    logWarnBME280Driver = LOG_SELECTOR(LOGGER_LEVEL_WARNING, LOGGER_MODULE_BME280_DRIVER),
    logWarnENS160Driver = LOG_SELECTOR(LOGGER_LEVEL_WARNING, LOGGER_MODULE_ENS160_DRIVER),
    logWarnEnvironmentalMon = LOG_SELECTOR(LOGGER_LEVEL_WARNING, LOGGER_MODULE_ENVIRONMENTAL_MON),
    logWarnTaskObject = LOG_SELECTOR(LOGGER_LEVEL_WARNING, LOGGER_MODULE_TASK_OBJECT),

    logNotifyMain = LOG_SELECTOR(LOGGER_LEVEL_NOTIFY, LOGGER_MODULE_MAIN),
    logNotifyDataModel = LOG_SELECTOR(LOGGER_LEVEL_NOTIFY, LOGGER_MODULE_DATA_MODEL),
    logNotifyMQTT = LOG_SELECTOR(LOGGER_LEVEL_NOTIFY, LOGGER_MODULE_MQTT),
    logNotifyNMEA = LOG_SELECTOR(LOGGER_LEVEL_NOTIFY, LOGGER_MODULE_NMEA),
    logNotifyDataModelBridge = LOG_SELECTOR(LOGGER_LEVEL_NOTIFY, LOGGER_MODULE_DATA_MODEL_BRIDGE),
    logNotifyNMEAWiFi = LOG_SELECTOR(LOGGER_LEVEL_NOTIFY, LOGGER_MODULE_NMEA_WIFI),
    logNotifyNMEAUART = LOG_SELECTOR(LOGGER_LEVEL_NOTIFY, LOGGER_MODULE_NMEA_UART),
    logNotifyNMEASoftUART = LOG_SELECTOR(LOGGER_LEVEL_NOTIFY, LOGGER_MODULE_NMEA_SOFT_UART),
    logNotifyNMEARMTUART = LOG_SELECTOR(LOGGER_LEVEL_NOTIFY, LOGGER_MODULE_NMEA_RMT_UART),
    logNotifySTALK = LOG_SELECTOR(LOGGER_LEVEL_NOTIFY, LOGGER_MODULE_STALK),
    logNotifySTALKUART = LOG_SELECTOR(LOGGER_LEVEL_NOTIFY, LOGGER_MODULE_STALK_UART),
    logNotifySTALKRMTUART = LOG_SELECTOR(LOGGER_LEVEL_NOTIFY, LOGGER_MODULE_STALK_RMT_UART),
    logNotifySeaTalk = LOG_SELECTOR(LOGGER_LEVEL_NOTIFY, LOGGER_MODULE_SEA_TALK),
    logNotifySeaTalkKRMTUART = LOG_SELECTOR(LOGGER_LEVEL_NOTIFY, LOGGER_MODULE_SEA_TALK_RMT_UART),
    logNotifyNMEAServer = LOG_SELECTOR(LOGGER_LEVEL_NOTIFY, LOGGER_MODULE_NMEA_SERVER),
    logNotifyNMEABridge = LOG_SELECTOR(LOGGER_LEVEL_NOTIFY, LOGGER_MODULE_NMEA_BRIDGE),
    logNotifyNMEALine = LOG_SELECTOR(LOGGER_LEVEL_NOTIFY, LOGGER_MODULE_NMEA_LINE),
    logNotifySeaTalkNMEABridge = LOG_SELECTOR(LOGGER_LEVEL_NOTIFY,
                                              LOGGER_MODULE_SEA_TALK_NMEA_BRIDGE),
    logNotifyUART = LOG_SELECTOR(LOGGER_LEVEL_NOTIFY, LOGGER_MODULE_UART),
    logNotifySoftUART = LOG_SELECTOR(LOGGER_LEVEL_NOTIFY, LOGGER_MODULE_SOFT_UART),
    logNotifyAIS = LOG_SELECTOR(LOGGER_LEVEL_NOTIFY, LOGGER_MODULE_AIS),
    logNotifyRMTUART = LOG_SELECTOR(LOGGER_LEVEL_NOTIFY, LOGGER_MODULE_RMT_UART),
    logNotifyWiFiInterface = LOG_SELECTOR(LOGGER_LEVEL_NOTIFY, LOGGER_MODULE_WIFI_INTERFACE),
    logNotifyWiFiManager = LOG_SELECTOR(LOGGER_LEVEL_NOTIFY, LOGGER_MODULE_WIFI_MANAGER),
    logNotifyStatsManager = LOG_SELECTOR(LOGGER_LEVEL_NOTIFY, LOGGER_MODULE_STATS_MANAGER),
    logNotifyI2CMaster = LOG_SELECTOR(LOGGER_LEVEL_NOTIFY, LOGGER_MODULE_I2C_MASTER),
    logNotifyBME280Driver = LOG_SELECTOR(LOGGER_LEVEL_NOTIFY, LOGGER_MODULE_BME280_DRIVER),
    logNotifyENS160Driver = LOG_SELECTOR(LOGGER_LEVEL_NOTIFY, LOGGER_MODULE_ENS160_DRIVER),
    logNotifyEnvironmentalMon = LOG_SELECTOR(LOGGER_LEVEL_NOTIFY, LOGGER_MODULE_ENVIRONMENTAL_MON),
    logNotifyTaskObject = LOG_SELECTOR(LOGGER_LEVEL_NOTIFY, LOGGER_MODULE_TASK_OBJECT),

    logErrorMain = LOG_SELECTOR(LOGGER_LEVEL_ERROR, LOGGER_MODULE_MAIN),
    logErrorDataModel = LOG_SELECTOR(LOGGER_LEVEL_ERROR, LOGGER_MODULE_DATA_MODEL),
    logErrorMQTT = LOG_SELECTOR(LOGGER_LEVEL_ERROR, LOGGER_MODULE_MQTT),
    logErrorNMEA = LOG_SELECTOR(LOGGER_LEVEL_ERROR, LOGGER_MODULE_NMEA),
    logErrorDataModelBridge = LOG_SELECTOR(LOGGER_LEVEL_ERROR, LOGGER_MODULE_DATA_MODEL_BRIDGE),
    logErrorNMEAWiFi = LOG_SELECTOR(LOGGER_LEVEL_ERROR, LOGGER_MODULE_NMEA_WIFI),
    logErrorNMEAUART = LOG_SELECTOR(LOGGER_LEVEL_ERROR, LOGGER_MODULE_NMEA_UART),
    logErrorNMEASoftUART = LOG_SELECTOR(LOGGER_LEVEL_ERROR, LOGGER_MODULE_NMEA_SOFT_UART),
    logErrorNMEARMTUART = LOG_SELECTOR(LOGGER_LEVEL_ERROR, LOGGER_MODULE_NMEA_RMT_UART),
    logErrorSTALK = LOG_SELECTOR(LOGGER_LEVEL_ERROR, LOGGER_MODULE_STALK),
    logErrorSTALKUART = LOG_SELECTOR(LOGGER_LEVEL_ERROR, LOGGER_MODULE_STALK_UART),
    logErrorSTALKRMTUART = LOG_SELECTOR(LOGGER_LEVEL_ERROR, LOGGER_MODULE_STALK_RMT_UART),
    logErrorSeaTalk = LOG_SELECTOR(LOGGER_LEVEL_ERROR, LOGGER_MODULE_SEA_TALK),
    logErrorSeaTalkRMTUART = LOG_SELECTOR(LOGGER_LEVEL_ERROR, LOGGER_MODULE_SEA_TALK_RMT_UART),
    logErrorNMEAServer = LOG_SELECTOR(LOGGER_LEVEL_ERROR, LOGGER_MODULE_NMEA_SERVER),
    logErrorNMEABridge = LOG_SELECTOR(LOGGER_LEVEL_ERROR, LOGGER_MODULE_NMEA_BRIDGE),
    logErrorNMEALine = LOG_SELECTOR(LOGGER_LEVEL_ERROR, LOGGER_MODULE_NMEA_LINE),
    logErrorSeaTalkNMEABridge = LOG_SELECTOR(LOGGER_LEVEL_ERROR,
                                             LOGGER_MODULE_SEA_TALK_NMEA_BRIDGE),
    logErrorUART = LOG_SELECTOR(LOGGER_LEVEL_ERROR, LOGGER_MODULE_UART),
    logErrorSoftUART = LOG_SELECTOR(LOGGER_LEVEL_ERROR, LOGGER_MODULE_SOFT_UART),
    logErrorAIS = LOG_SELECTOR(LOGGER_LEVEL_ERROR, LOGGER_MODULE_AIS),
    logErrorRMTUART = LOG_SELECTOR(LOGGER_LEVEL_ERROR, LOGGER_MODULE_RMT_UART),
    logErrorWiFiInterface = LOG_SELECTOR(LOGGER_LEVEL_ERROR, LOGGER_MODULE_WIFI_INTERFACE),
    logErrorWiFiManager = LOG_SELECTOR(LOGGER_LEVEL_ERROR, LOGGER_MODULE_WIFI_MANAGER),
    logErrorStatsManager = LOG_SELECTOR(LOGGER_LEVEL_ERROR, LOGGER_MODULE_STATS_MANAGER),
    logErrorI2CMaster = LOG_SELECTOR(LOGGER_LEVEL_ERROR, LOGGER_MODULE_I2C_MASTER),
    logErrorBME280Driver = LOG_SELECTOR(LOGGER_LEVEL_ERROR, LOGGER_MODULE_BME280_DRIVER),
    logErrorENS160Driver = LOG_SELECTOR(LOGGER_LEVEL_ERROR, LOGGER_MODULE_ENS160_DRIVER),
    logErrorEnvironmentalMon = LOG_SELECTOR(LOGGER_LEVEL_ERROR, LOGGER_MODULE_ENVIRONMENTAL_MON),
    logErrorTaskObject = LOG_SELECTOR(LOGGER_LEVEL_ERROR, LOGGER_MODULE_TASK_OBJECT),
};

enum LogBase {
    Dec,
    Hex
};

struct EndOfLine {};
const EndOfLine eol = EndOfLine();

class Logger {
    private:
        static const size_t maxLogEntryLength = 128;

        LoggerLevel logLevel;
        LoggerLevel lineLevel;
        LoggerModule lineModule;
        bool outputCurrentLine;
        bool moduleDebugFlags[LOGGER_MODULE_COUNT];

        LogBase base;
        uint8_t errorsSetInDataModel;
        etl::string<maxLogEntryLength> errorLine;
        // Flag used to make sure that we don't try to set an error in the DataModel that occured
        // while trying to set an error in the DataModel.
        bool inLogger;

        const char *moduleName(LoggerModule module);
        void logString(const char *string);
        void logCharacter(char character);
        void addErrorLineToDebugs();
        void scrollUpDebugs();

    public:
        Logger(LoggerLevel level);
        void setLevel(LoggerLevel level);
        void enableModuleDebug(LoggerModule module);
        void disableModuleDebug(LoggerModule module);
        bool debugEnabled(LoggerModule module);
        void initForTask();
        Logger & operator << (const LogSelector level);
        Logger & operator << (const LogBase base);
        Logger & operator << (char character);
        Logger & operator << (const char *string);
        Logger & operator << (const etl::istring &string);
        Logger & operator << (const etl::string_view &stringView);
        Logger & operator << (uint8_t value);
        Logger & operator << (uint16_t value);
        Logger & operator << (uint32_t value);
        Logger & operator << (unsigned value);
        Logger & operator << (int16_t value);
        Logger & operator << (int32_t value);
        Logger & operator << (int value);
        Logger & operator << (bool value);
        Logger & operator << (float value);
        Logger & operator << (esp_ip4_addr_t addr);
        Logger & operator << (struct in_addr addr);
        Logger & operator << (struct sockaddr_in addr);

        Logger & operator << (const LoggableItem &item);
        Logger & operator << (const EndOfLine &eol);
};

extern __thread Logger *threadSpecificLogger;

inline Logger &logger() {
    return *threadSpecificLogger;
}

inline Logger &taskLogger() {
    return *threadSpecificLogger;
}

#endif // LOGGER_H
