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

#include "I2CMaster.h"
#include "BME280Driver.h"
#include "ENS160Driver.h"
#include "StatusLED.h"
#include "HundredthsInt16.h"
#include "HundredthsUInt8.h"
#include "HundredthsUInt32.h"
#include "Logger.h"
#include "Error.h"

#include "freertos/portmacro.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define STATUS_LED_GPIO     ((gpio_num_t)CONFIG_STATUS_LED_GPIO)
#define I2C_MASTER_NUM      ((i2c_port_t)CONFIG_I2C_MASTER_NUM)
#define I2C_MASTER_SCL_IO   ((gpio_num_t)CONFIG_I2C_MASTER_SCL_IO)
#define I2C_MASTER_SDA_IO   ((gpio_num_t)CONFIG_I2C_MASTER_SDA_IO)

extern "C" void app_main(void) {
    StatusLED statusLED(STATUS_LED_GPIO);

    bool bme280Functional;
    bool ens160Functional;

    logger.setLevel(LOGGER_LEVEL_DEBUG);
//    logger.enableModuleDebug(LOGGER_MODULE_BME280_DRIVER);
//    logger.enableModuleDebug(LOGGER_MODULE_ENS160_DRIVER);

    I2CMaster ic2Master(I2C_MASTER_NUM, I2C_MASTER_SCL_IO, I2C_MASTER_SDA_IO);

    BME280Driver bme280Driver(ic2Master, CONFIG_BME280_ADDRESS);

    if (bme280Driver.detect()) {
        bme280Functional = true;
        logger << logNotifyMain << "BME280 detected." << eol;
        try {
            bme280Driver.start();
        }
        catch (esp_err_t error) {
            logger << logErrorMain << "Failed to start BME280 device." << eol;
            bme280Functional = false;
        }
    } else {
        logger << logErrorMain << "BME280 not detected." << eol;
        bme280Functional = false;
    }

    ENS160Driver ens160Driver(ic2Master, CONFIG_ENS160_ADDRESS);
    if (ens160Driver.detect()) {
        ens160Functional = true;
        logger << logNotifyMain << "ENS160 detected." << eol;
        try {
            ens160Driver.start();
        }
        catch (esp_err_t error) {
            logger << logErrorMain << "Failed to start ENS160 device." << eol;
            statusLED.off();
            ens160Functional = false;
        }
    } else {
        logger << logErrorMain << "ENS160 not detected." << eol;
        statusLED.off();
        ens160Functional = false;
    }

    while (1) {
        if (bme280Functional) {
            HundredthsInt16 temperatureC;
            HundredthsUInt32 pressureMBar;
            HundredthsUInt8 relativeHumidity;
            bme280Driver.read(temperatureC, pressureMBar, relativeHumidity);

            HundredthsInt16 temperatureF = (temperatureC * 9) / 5 + 32;

            logger << logNotifyMain << "Temperature = " << temperatureF << " F"
                   << " (" << temperatureC << " C)"
                   << "  Pressure = " << pressureMBar << " mBar"
                   << "  Relative Humidity = " << relativeHumidity << "%" << eol;
        }

        if (ens160Functional) {
            bool ens160StartingUp;
            uint8_t aqi;
            uint16_t tvoc;
            uint16_t eco2;
            bool ens160HasData = ens160Driver.read(ens160StartingUp, aqi, tvoc, eco2);
            if (ens160HasData) {
                logger << logNotifyMain
                       << "AQI = " << aqi << " (" << ens160Driver.aqiDescription(aqi) << ")"
                       << "  TVOC = " << tvoc << " (" << ens160Driver.tvocDescription(tvoc) << ")"
                       << "  eCO2 = " << eco2 << " (" << ens160Driver.eco2Description(eco2) << ")";
                if (ens160StartingUp) {
                    logger << "  Starting Up";
                }
                logger << eol;

                if (aqi >= 4) {
                    statusLED.errorFlash();
                } else if (ens160StartingUp) {
                    statusLED.normalFlash();
                } else {
                    statusLED.on();
                }
            } else {
                logger << logNotifyMain << "No valid environmental data." << eol;
                statusLED.off();
            }
        }

        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
}
