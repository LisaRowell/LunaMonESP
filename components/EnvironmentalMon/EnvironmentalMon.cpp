/*
 * This file is part of LunaMon (https://github.com/LisaRowell/LunaMonESP)
 * Copyright (C) 2023-2024 Lisa Rowell
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

#include "EnvironmentalMon.h"

#include "DataModel.h"
#include "DataModelUInt8Leaf.h"
#include "DataModelUInt16Leaf.h"
#include "DataModelHundredthsInt16Leaf.h"
#include "DataModelHundredthsUInt8Leaf.h"
#include "DataModelHundredthsUInt32Leaf.h"

#include "I2CMaster.h"
#include "StatusLED.h"

#include "BME280Driver.h"
#include "ENS160Driver.h"

#include "HundredthsUInt8.h"
#include "HundredthsInt16.h"
#include "HundredthsUInt32.h"

#include "Logger.h"

#include "Error.h"

#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"
#include "freertos/task.h"

#define STACK_SIZE  (8 * 1024)

#if !CONFIG_LUNAMON_BME280_ENABLED
#define CONFIG_LUNAMON_BME280_ADDRESS   0
#endif
#if !CONFIG_LUNAMON_ENS160_ENABLED
#define CONFIG_LUNAMON_ENS160_ADDRESS   0
#endif

EnvironmentalMon::EnvironmentalMon(DataModel &dataModel, I2CMaster &ic2Master, StatusLED *statusLED)
    : TaskObject("EnvironmentalMon", LOGGER_LEVEL_DEBUG, STACK_SIZE),
      environmentDataModelNode("environment", &dataModel.rootNode()),
      cabinDataModelNode("cabin", &environmentDataModelNode),
      temperatureCLeaf("temperatureC", &cabinDataModelNode),
      temperatureFLeaf("temperatureF", &cabinDataModelNode),
      pressureLeaf("pressure", &cabinDataModelNode),
      relativeHumidityLeaf("relativeHumidity", &cabinDataModelNode),
      aqiDataModelLeaf("aqi", &cabinDataModelNode),
      tvocDataModelLeaf("tvoc", &cabinDataModelNode),
      eco2DataModelLeaf("eco2", &cabinDataModelNode),
      statusLED(statusLED) {
    if (CONFIG_LUNAMON_BME280_ENABLED) {
        bme280Driver = new BME280Driver(ic2Master, CONFIG_LUNAMON_BME280_ADDRESS);
    } else {
        bme280Driver = nullptr;
    }

    if (CONFIG_LUNAMON_ENS160_ENABLED) {
        ens160Driver = new ENS160Driver(ic2Master, CONFIG_LUNAMON_ENS160_ADDRESS);
    } else {
        ens160Driver = nullptr;
    }
}

void EnvironmentalMon::task() {
    if (bme280Driver) {
        detectBME280();
    } else {
        bme280Functional = false;
    }

    if (ens160Driver) {
        detectENS160();
    } else {
        ens160Functional = false;
    }

    while (1) {
        if (bme280Functional) {
            pollBME280();
        }

        if (ens160Functional) {
            pollENS160();
        }

        vTaskDelay(pdMS_TO_TICKS(2000));
    }
}

void EnvironmentalMon::detectBME280() {
    if (bme280Driver->detect()) {
        bme280Functional = true;
        logger << logNotifyEnvironmentalMon << "BME280 detected." << eol;
        try {
            bme280Driver->start();
        }
        catch (esp_err_t error) {
            logger << logErrorEnvironmentalMon << "Failed to start BME280 device." << eol;
            bme280Functional = false;
        }
    } else {
        logger << logErrorEnvironmentalMon << "BME280 not detected." << eol;
        bme280Functional = false;
    }
}

void EnvironmentalMon::pollBME280() {
    HundredthsInt16 temperatureC;
    HundredthsUInt32 pressureMBar;
    HundredthsUInt8 relativeHumidity;
    bme280Driver->read(temperatureC, pressureMBar, relativeHumidity);

    HundredthsInt16 temperatureF = (temperatureC * 9) / 5 + 32;

    temperatureCLeaf = temperatureC;
    temperatureFLeaf = temperatureF;
    pressureLeaf = pressureMBar;
    relativeHumidityLeaf = relativeHumidity;

    logger << logDebugEnvironmentalMon << "Temperature = " << temperatureF << " F"
           << " (" << temperatureC << " C)"
           << "  Pressure = " << pressureMBar << " mBar"
           << "  Relative Humidity = " << relativeHumidity << "%" << eol;
}

void EnvironmentalMon::detectENS160() {
    if (ens160Driver->detect()) {
        ens160Functional = true;
        logger << logNotifyEnvironmentalMon << "ENS160 detected." << eol;
        try {
            ens160Driver->start();
        }
        catch (esp_err_t error) {
            logger << logErrorEnvironmentalMon << "Failed to start ENS160 device." << eol;
            statusLEDOff();
            ens160Functional = false;
        }
    } else {
        logger << logErrorEnvironmentalMon << "ENS160 not detected." << eol;
        statusLEDOff();
        ens160Functional = false;
    }
}

void EnvironmentalMon::pollENS160() {
    bool ens160StartingUp;
    uint8_t aqi;
    uint16_t tvoc;
    uint16_t eco2;
    bool ens160HasData = ens160Driver->read(ens160StartingUp, aqi, tvoc, eco2);
    if (ens160HasData) {
        logger << logDebugEnvironmentalMon
               << "AQI = " << aqi << " (" << ens160Driver->aqiDescription(aqi) << ")"
               << "  TVOC = " << tvoc << " (" << ens160Driver->tvocDescription(tvoc) << ")"
               << "  eCO2 = " << eco2 << " (" << ens160Driver->eco2Description(eco2) << ")";
        if (ens160StartingUp) {
            logger << "  Starting Up";
        }
        logger << eol;

        aqiDataModelLeaf = aqi;
        tvocDataModelLeaf = tvoc;
        eco2DataModelLeaf = eco2;

        if (aqi >= 4) {
            statusErrorFlash();
        } else if (ens160StartingUp) {
            statusNormalFlash();
        } else {
            statusLEDOn();
        }
    } else {
        logger << logDebugEnvironmentalMon << "No valid environmental data." << eol;

        aqiDataModelLeaf.clear();

        statusLEDOff();
    }
}

void EnvironmentalMon::statusLEDOn() {
    if (statusLED) {
        statusLED->on();
    }
}

void EnvironmentalMon::statusLEDOff() {
    if (statusLED) {
        statusLED->off();
    }
}

void EnvironmentalMon::statusNormalFlash() {
    if (statusLED) {
        statusLED->normalFlash();
    }
}

void EnvironmentalMon::statusErrorFlash() {
    if (statusLED) {
        statusLED->errorFlash();
    }
}
