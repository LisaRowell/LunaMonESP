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

#include "Config.h"
#include "I2CMaster.h"
#include "BME280Driver.h"
#include "HundredthsInt16.h"
#include "HundredthsUInt8.h"
#include "HundredthsUInt32.h"
#include "Logger.h"
#include "Error.h"

#include "freertos/portmacro.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

extern "C" void app_main(void) {
    logger.setLevel(LOGGER_LEVEL_DEBUG);
    logger.enableModuleDebug(LOGGER_MODULE_BME280_DRIVER);

    I2CMaster ic2Master(I2C_MASTER_NUM, I2C_MASTER_SCL_IO, I2C_MASTER_SDA_IO);
    BME280Driver bme280Driver(ic2Master, BME280_ADDRESS);

    if (bme280Driver.detect()) {
        logger << logNotifyMain << "BME280 detected." << eol;
        try {
            bme280Driver.start();
        }
        catch (esp_err_t error) {
            logger << logErrorMain << "Failed to start BME280 device." << eol;
        }
    } else {
        logger << logErrorMain << "BME280 not detected." << eol;
    }

    while (1) {
        HundredthsInt16 temperatureC;
        HundredthsUInt32 pressureMBar;
        HundredthsUInt8 relativeHumidity;
        bme280Driver.read(temperatureC, pressureMBar, relativeHumidity);

        HundredthsInt16 temperatureF = (temperatureC * 9) / 5 + 32;

        logger << logNotifyMain << "Temperature = " << temperatureF << " F"
               << " (" << temperatureC << " C)"
               << "  Pressure = " << pressureMBar << " mBar"
               << "  Relative Humidity = " << relativeHumidity << "%" << eol;

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
