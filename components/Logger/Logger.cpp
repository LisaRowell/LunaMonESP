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

#include "Logger.h"
#include "LoggableItem.h"

#include "Error.h"

#include <etl/string.h>
#include <etl/string_view.h>

#include "esp_log.h"

#include <stdint.h>

Logger logger(LOGGER_LEVEL_WARNING);

Logger::Logger(LoggerLevel level)
    : logLevel(level), lineLevel(LOGGER_LEVEL_ERROR), lineModule(LOGGER_MODULE_UTIL),
      outputCurrentLine(false), base(Dec), errorsSetInDataModel(0), inLogger(false) {
    errorLine.clear();

    unsigned moduleIndex;
    for (moduleIndex = 0; moduleIndex < LOGGER_MODULE_COUNT; moduleIndex++) {
        moduleDebugFlags[moduleIndex] = false;
    }
}

void Logger::setLevel(LoggerLevel level) {
    logLevel = level;
}

void Logger::enableModuleDebug(LoggerModule module) {
    moduleDebugFlags[module] = true;
}

void Logger::disableModuleDebug(LoggerModule module) {
    moduleDebugFlags[module] = false;
}

Logger & Logger::operator << (const LogSelector logSelector) {
    lineLevel = (LoggerLevel)((uint16_t)logSelector >> LOG_LEVEL_SHIFT);
    lineModule = (LoggerModule)((uint16_t)logSelector & LOGGER_MODULE_MASK);
    if (lineLevel >= logLevel) {
        if (lineLevel == LOGGER_LEVEL_DEBUG) {
            outputCurrentLine = moduleDebugFlags[lineModule];
        } else {
            outputCurrentLine = true;
        }
    } else {
        outputCurrentLine = false;
    }

    return *this;
}

Logger & Logger::operator << (const LogBase base) {
    Logger::base = base;

    return *this;
}

Logger & Logger::operator << (char character) {
    if (outputCurrentLine) {
        logCharacter(character);
    }

    return *this;
}

Logger & Logger::operator << (const char *string) {
    if (outputCurrentLine) {
        if (string != NULL) {
            logString(string);
        } else {
            logString("(nil)");
        }
    }

    return *this;
}

Logger & Logger::operator << (const etl::istring &string) {
    if (outputCurrentLine) {
       logString(string.data());
    }

    return *this;
}

Logger & Logger::operator << (const etl::string_view &stringView) {
    if (outputCurrentLine) {
        for (char character: stringView) {
            logCharacter(character);
        }
    }

    return *this;
}

Logger & Logger::operator << (uint8_t value) {
    if (outputCurrentLine) {
        switch (base) {
            case Dec:
                char decimalStr[4];
                snprintf(decimalStr, 4, "%u", value);
                logString(decimalStr);
                break;

            case Hex:
                char hexStr[5];
                snprintf(hexStr, 5, "0x%0x", value);
                logString(hexStr);
                break;

            default:
                fatalError("Messed up base in logger");
        }
    }

    return *this;
}

Logger & Logger::operator << (uint16_t value) {
    if (outputCurrentLine) {
        switch (base) {
            case Dec:
                char decimalStr[6];
                snprintf(decimalStr, 6, "%u", value);
                logString(decimalStr);
                break;

            case Hex:
                char hexStr[7];
                snprintf(hexStr, 7, "0x%0x", value);
                logString(hexStr);
                break;

            default:
                fatalError("Messed up base in logger");
        }
    }

    return *this;
}

Logger & Logger::operator << (uint32_t value) {
    if (outputCurrentLine) {
        switch (base) {
            case Dec:
                char decimalStr[11];
                snprintf(decimalStr, 11, "%lu", value);
                logString(decimalStr);
                break;

            case Hex:
                char hexStr[11];
                snprintf(hexStr, 11, "0x%0lx", value);
                logString(hexStr);
                break;

            default:
                fatalError("Messed up base in logger");
        }
    }

    return *this;
}

Logger & Logger::operator << (unsigned value) {
    if (outputCurrentLine) {
        switch (base) {
            case Dec:
                char decimalStr[11];
                snprintf(decimalStr, 11, "%u", value);
                logString(decimalStr);
                break;

            case Hex:
                char hexStr[11];
                snprintf(hexStr, 11, "0x%0x", value);
                logString(hexStr);
                break;

            default:
                fatalError("Messed up base in logger");
        }
    }

    return *this;
}

Logger & Logger::operator << (int16_t value) {
    if (outputCurrentLine) {
        switch (base) {
            case Dec:
                char decimalStr[12];
                snprintf(decimalStr, 12, "%d", value);
                logString(decimalStr);
                break;

            case Hex:
                char hexStr[11];
                snprintf(hexStr, 11, "0x%0x", value);
                logString(hexStr);
                break;

            default:
                fatalError("Messed up base in logger");
        }
    }

    return *this;
}

Logger & Logger::operator << (int32_t value) {
    if (outputCurrentLine) {
        switch (base) {
            case Dec:
                char decimalStr[16];
                snprintf(decimalStr, 16, "%ld", value);
                logString(decimalStr);
                break;

            case Hex:
                char hexStr[16];
                snprintf(hexStr, 16, "0x%0lx", value);
                logString(hexStr);
                break;

            default:
                fatalError("Messed up base in logger");
        }
    }

    return *this;
}

Logger & Logger::operator << (int value) {
    if (outputCurrentLine) {
        switch (base) {
            case Dec:
                char decimalStr[12];
                snprintf(decimalStr, 12, "%d", value);
                logString(decimalStr);
                break;

            case Hex:
                char hexStr[11];
                snprintf(hexStr, 11, "0x%0x", value);
                logString(hexStr);
                break;

            default:
                fatalError("Messed up base in logger");
        }
    }

    return *this;
}

Logger & Logger::operator << (bool value) {
    if (outputCurrentLine) {
        if (value) {
            logString("true");
        } else {
            logString("false");
        }
    }

    return *this;
}

Logger & Logger::operator << (const LoggableItem &item) {
    if (outputCurrentLine) {
        item.log(*this);
    }

    return *this;
}

Logger & Logger::operator << (const EndOfLine &eol) {
    base = Dec;
 
    if (outputCurrentLine) {
        ESP_LOGI(moduleName(lineModule), "%s", errorLine.data());
    }

    if (!inLogger) {
        errorLine.clear();
    }

    return *this;
}

const char *Logger::moduleName(LoggerModule module) {
    switch (module) {
        case LOGGER_MODULE_MAIN:
            return "Main";
        case LOGGER_MODULE_DATA_MODEL:
            return "Data Model";
        case LOGGER_MODULE_MQTT:
            return "MQTT";
        case LOGGER_MODULE_NMEA:
            return "NMEA";
        case LOGGER_MODULE_NMEA_DATA_MODEL_BRIDGE:
            return "Data Model Bridge";
        case LOGGER_MODULE_NMEA_WIFI_BRIDGE:
            return "NMEA WiFi Bridge";
        case LOGGER_MODULE_NMEA_WIFI:
            return "NMEA WiFi";
        case LOGGER_MODULE_UTIL:
            return "Util";
        case LOGGER_MODULE_WIFI_MANAGER:
            return "WiFi Manager";
        case LOGGER_MODULE_STATS_MANAGER:
            return "Stats Manager";
        case LOGGER_MODULE_I2C_MASTER:
            return "I2C Master";
        case LOGGER_MODULE_BME280_DRIVER:
            return "BME280 Driver";
        case LOGGER_MODULE_ENS160_DRIVER:
            return "ENS160 Driver";
        default:
            fatalError("Unhandled LoggerModule in Logger::moduleName");
    }
}

void Logger::logString(const char *string) {
    errorLine.append(string);
}

void Logger::logCharacter(char character) {
    errorLine.append(1, character);
}
