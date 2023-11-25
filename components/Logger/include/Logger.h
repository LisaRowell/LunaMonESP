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

#ifndef LOGGER_H
#define LOGGER_H

#include <etl/string.h>
#include <etl/string_view.h>

#include "esp_netif.h"

#include <stdint.h>

class LoggableItem;

enum LoggerModule {
    LOGGER_MODULE_MAIN,
    LOGGER_MODULE_DATA_MODEL,
    LOGGER_MODULE_MQTT,
    LOGGER_MODULE_NMEA,
    LOGGER_MODULE_NMEA_DATA_MODEL_BRIDGE,
    LOGGER_MODULE_NMEA_WIFI_BRIDGE,
    LOGGER_MODULE_NMEA_WIFI,
    LOGGER_MODULE_UTIL,
    LOGGER_MODULE_WIFI_MANAGER,
    LOGGER_MODULE_STATS_MANAGER,
    LOGGER_MODULE_I2C_MASTER,
    LOGGER_MODULE_BME280_DRIVER,
    LOGGER_MODULE_ENS160_DRIVER,
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
    logDebugUtil = LOG_SELECTOR(LOGGER_LEVEL_DEBUG, LOGGER_MODULE_UTIL),
    logDebugWiFiManager = LOG_SELECTOR(LOGGER_LEVEL_DEBUG, LOGGER_MODULE_WIFI_MANAGER),
    logDebugStatsManager = LOG_SELECTOR(LOGGER_LEVEL_DEBUG, LOGGER_MODULE_STATS_MANAGER),
    logDebugI2CMaster = LOG_SELECTOR(LOGGER_LEVEL_DEBUG, LOGGER_MODULE_I2C_MASTER),
    logDebugBME280Driver = LOG_SELECTOR(LOGGER_LEVEL_DEBUG, LOGGER_MODULE_BME280_DRIVER),
    logDebugENS160Driver = LOG_SELECTOR(LOGGER_LEVEL_DEBUG, LOGGER_MODULE_ENS160_DRIVER),
    logDebugTaskObject = LOG_SELECTOR(LOGGER_LEVEL_DEBUG, LOGGER_MODULE_TASK_OBJECT),

    logWarnMain = LOG_SELECTOR(LOGGER_LEVEL_WARNING, LOGGER_MODULE_MAIN),
    logWarnDataModel = LOG_SELECTOR(LOGGER_LEVEL_WARNING, LOGGER_MODULE_DATA_MODEL),
    logWarnMQTT = LOG_SELECTOR(LOGGER_LEVEL_WARNING, LOGGER_MODULE_MQTT),
    logWarnNMEA = LOG_SELECTOR(LOGGER_LEVEL_WARNING, LOGGER_MODULE_NMEA),
    logWarnNMEADataModelBridge =
        LOG_SELECTOR(LOGGER_LEVEL_WARNING, LOGGER_MODULE_NMEA_DATA_MODEL_BRIDGE),
    logWarnNMEAWiFi = LOG_SELECTOR(LOGGER_LEVEL_WARNING, LOGGER_MODULE_NMEA_WIFI),
    logWarnUtil = LOG_SELECTOR(LOGGER_LEVEL_WARNING, LOGGER_MODULE_UTIL),
    logWarnWiFiManager = LOG_SELECTOR(LOGGER_LEVEL_WARNING, LOGGER_MODULE_WIFI_MANAGER),
    logWarnStatsManager = LOG_SELECTOR(LOGGER_LEVEL_WARNING, LOGGER_MODULE_STATS_MANAGER),
    logWarnI2CMaster = LOG_SELECTOR(LOGGER_LEVEL_WARNING, LOGGER_MODULE_I2C_MASTER),
    logWarnBME280Driver = LOG_SELECTOR(LOGGER_LEVEL_WARNING, LOGGER_MODULE_BME280_DRIVER),
    logWarnENS160Driver = LOG_SELECTOR(LOGGER_LEVEL_WARNING, LOGGER_MODULE_ENS160_DRIVER),
    logWarnTaskObject = LOG_SELECTOR(LOGGER_LEVEL_WARNING, LOGGER_MODULE_TASK_OBJECT),

    logNotifyMain = LOG_SELECTOR(LOGGER_LEVEL_NOTIFY, LOGGER_MODULE_MAIN),
    logNotifyDataModel = LOG_SELECTOR(LOGGER_LEVEL_NOTIFY, LOGGER_MODULE_DATA_MODEL),
    logNotifyMQTT = LOG_SELECTOR(LOGGER_LEVEL_NOTIFY, LOGGER_MODULE_MQTT),
    logNotifyNMEA = LOG_SELECTOR(LOGGER_LEVEL_NOTIFY, LOGGER_MODULE_NMEA),
    logNotifyNMEADataModelBridge =
        LOG_SELECTOR(LOGGER_LEVEL_NOTIFY, LOGGER_MODULE_NMEA_DATA_MODEL_BRIDGE),
    logNotifyNMEAWiFi = LOG_SELECTOR(LOGGER_LEVEL_NOTIFY, LOGGER_MODULE_NMEA_WIFI),
    logNotifyUtil = LOG_SELECTOR(LOGGER_LEVEL_NOTIFY, LOGGER_MODULE_UTIL),
    logNotifyWiFiManager = LOG_SELECTOR(LOGGER_LEVEL_NOTIFY, LOGGER_MODULE_WIFI_MANAGER),
    logNotifyStatsManager = LOG_SELECTOR(LOGGER_LEVEL_NOTIFY, LOGGER_MODULE_STATS_MANAGER),
    logNotifyI2CMaster = LOG_SELECTOR(LOGGER_LEVEL_NOTIFY, LOGGER_MODULE_I2C_MASTER),
    logNotifyBME280Driver = LOG_SELECTOR(LOGGER_LEVEL_NOTIFY, LOGGER_MODULE_BME280_DRIVER),
    logNotifyENS160Driver = LOG_SELECTOR(LOGGER_LEVEL_NOTIFY, LOGGER_MODULE_ENS160_DRIVER),
    logNotifyTaskObject = LOG_SELECTOR(LOGGER_LEVEL_NOTIFY, LOGGER_MODULE_TASK_OBJECT),

    logErrorMain = LOG_SELECTOR(LOGGER_LEVEL_ERROR, LOGGER_MODULE_MAIN),
    logErrorDataModel = LOG_SELECTOR(LOGGER_LEVEL_ERROR, LOGGER_MODULE_DATA_MODEL),
    logErrorMQTT = LOG_SELECTOR(LOGGER_LEVEL_ERROR, LOGGER_MODULE_MQTT),
    logErrorNMEA = LOG_SELECTOR(LOGGER_LEVEL_ERROR, LOGGER_MODULE_NMEA),
    logErrorNMEADataModelBridge =
        LOG_SELECTOR(LOGGER_LEVEL_ERROR, LOGGER_MODULE_NMEA_DATA_MODEL_BRIDGE),
    logErrorNMEAWiFi = LOG_SELECTOR(LOGGER_LEVEL_ERROR, LOGGER_MODULE_NMEA_WIFI),
    logErrorUtil = LOG_SELECTOR(LOGGER_LEVEL_ERROR, LOGGER_MODULE_UTIL),
    logErrorWiFiManager = LOG_SELECTOR(LOGGER_LEVEL_ERROR, LOGGER_MODULE_WIFI_MANAGER),
    logErrorStatsManager = LOG_SELECTOR(LOGGER_LEVEL_ERROR, LOGGER_MODULE_STATS_MANAGER),
    logErrorI2CMaster = LOG_SELECTOR(LOGGER_LEVEL_ERROR, LOGGER_MODULE_I2C_MASTER),
    logErrorBME280Driver = LOG_SELECTOR(LOGGER_LEVEL_ERROR, LOGGER_MODULE_BME280_DRIVER),
    logErrorENS160Driver = LOG_SELECTOR(LOGGER_LEVEL_ERROR, LOGGER_MODULE_ENS160_DRIVER),
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
        Logger & operator << (esp_ip4_addr_t addr);

        Logger & operator << (const LoggableItem &item);
        Logger & operator << (const EndOfLine &eol);
};

extern __thread Logger *threadSpecificLogger;

inline Logger &logger() {
    return *threadSpecificLogger;
}

#endif // LOGGER_H
