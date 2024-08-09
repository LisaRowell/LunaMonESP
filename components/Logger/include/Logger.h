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
    LOGGER_MODULE_NMEA_DATA_MODEL_BRIDGE,
    LOGGER_MODULE_NMEA_WIFI,
    LOGGER_MODULE_NMEA_UART,
    LOGGER_MODULE_STALK,
    LOGGER_MODULE_STALK_UART,
    LOGGER_MODULE_SEATALK,
    LOGGER_MODULE_NMEA_LINE,
    LOGGER_MODULE_UART,
    LOGGER_MODULE_AIS,
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
    logDebugNMEADataModelBridge =
        LOG_SELECTOR(LOGGER_LEVEL_DEBUG, LOGGER_MODULE_NMEA_DATA_MODEL_BRIDGE),
    logDebugNMEAWiFi = LOG_SELECTOR(LOGGER_LEVEL_DEBUG, LOGGER_MODULE_NMEA_WIFI),
    logDebugNMEAUART = LOG_SELECTOR(LOGGER_LEVEL_DEBUG, LOGGER_MODULE_NMEA_UART),
    logDebugSTALK = LOG_SELECTOR(LOGGER_LEVEL_DEBUG, LOGGER_MODULE_STALK),
    logDebugSTALKUART = LOG_SELECTOR(LOGGER_LEVEL_DEBUG, LOGGER_MODULE_STALK_UART),
    logDebugSeaTalk = LOG_SELECTOR(LOGGER_LEVEL_DEBUG, LOGGER_MODULE_SEATALK),
    logDebugNMEALine = LOG_SELECTOR(LOGGER_LEVEL_DEBUG, LOGGER_MODULE_NMEA_LINE),
    logDebugUART = LOG_SELECTOR(LOGGER_LEVEL_DEBUG, LOGGER_MODULE_UART),
    logDebugAIS = LOG_SELECTOR(LOGGER_LEVEL_DEBUG, LOGGER_MODULE_AIS),
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
    logWarnNMEADataModelBridge =
        LOG_SELECTOR(LOGGER_LEVEL_WARNING, LOGGER_MODULE_NMEA_DATA_MODEL_BRIDGE),
    logWarnNMEAWiFi = LOG_SELECTOR(LOGGER_LEVEL_WARNING, LOGGER_MODULE_NMEA_WIFI),
    logWarnNMEAUART = LOG_SELECTOR(LOGGER_LEVEL_WARNING, LOGGER_MODULE_NMEA_UART),
    logWarnSTALK = LOG_SELECTOR(LOGGER_LEVEL_WARNING, LOGGER_MODULE_STALK),
    logWarnSTALKUART = LOG_SELECTOR(LOGGER_LEVEL_WARNING, LOGGER_MODULE_STALK_UART),
    logWarnSeaTalk = LOG_SELECTOR(LOGGER_LEVEL_WARNING, LOGGER_MODULE_SEATALK),
    logWarnNMEALine = LOG_SELECTOR(LOGGER_LEVEL_WARNING, LOGGER_MODULE_NMEA_LINE),
    logWarnUART = LOG_SELECTOR(LOGGER_LEVEL_WARNING, LOGGER_MODULE_UART),
    logWarnAIS = LOG_SELECTOR(LOGGER_LEVEL_WARNING, LOGGER_MODULE_AIS),
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
    logNotifyNMEADataModelBridge =
        LOG_SELECTOR(LOGGER_LEVEL_NOTIFY, LOGGER_MODULE_NMEA_DATA_MODEL_BRIDGE),
    logNotifyNMEAWiFi = LOG_SELECTOR(LOGGER_LEVEL_NOTIFY, LOGGER_MODULE_NMEA_WIFI),
    logNotifyNMEAUART = LOG_SELECTOR(LOGGER_LEVEL_NOTIFY, LOGGER_MODULE_NMEA_UART),
    logNotifySTALK = LOG_SELECTOR(LOGGER_LEVEL_NOTIFY, LOGGER_MODULE_STALK),
    logNotifySTALKUART = LOG_SELECTOR(LOGGER_LEVEL_NOTIFY, LOGGER_MODULE_STALK_UART),
    logNotifySeaTalk = LOG_SELECTOR(LOGGER_LEVEL_NOTIFY, LOGGER_MODULE_SEATALK),
    logNotifyNMEALine = LOG_SELECTOR(LOGGER_LEVEL_NOTIFY, LOGGER_MODULE_NMEA_LINE),
    logNotifyUART = LOG_SELECTOR(LOGGER_LEVEL_NOTIFY, LOGGER_MODULE_UART),
    logNotifyAIS = LOG_SELECTOR(LOGGER_LEVEL_NOTIFY, LOGGER_MODULE_AIS),
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
    logErrorNMEADataModelBridge =
        LOG_SELECTOR(LOGGER_LEVEL_ERROR, LOGGER_MODULE_NMEA_DATA_MODEL_BRIDGE),
    logErrorNMEAWiFi = LOG_SELECTOR(LOGGER_LEVEL_ERROR, LOGGER_MODULE_NMEA_WIFI),
    logErrorNMEAUART = LOG_SELECTOR(LOGGER_LEVEL_ERROR, LOGGER_MODULE_NMEA_UART),
    logErrorSTALK = LOG_SELECTOR(LOGGER_LEVEL_ERROR, LOGGER_MODULE_STALK),
    logErrorSTALKUART = LOG_SELECTOR(LOGGER_LEVEL_ERROR, LOGGER_MODULE_STALK_UART),
    logErrorSeaTalk = LOG_SELECTOR(LOGGER_LEVEL_ERROR, LOGGER_MODULE_SEATALK),
    logErrorNMEALine = LOG_SELECTOR(LOGGER_LEVEL_ERROR, LOGGER_MODULE_NMEA_LINE),
    logErrorUART = LOG_SELECTOR(LOGGER_LEVEL_ERROR, LOGGER_MODULE_UART),
    logErrorAIS = LOG_SELECTOR(LOGGER_LEVEL_ERROR, LOGGER_MODULE_AIS),
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
