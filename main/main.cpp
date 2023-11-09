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
#include "Logger.h"
#include "Error.h"

#include "freertos/portmacro.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "driver/i2c.h"

#define I2C_MASTER_SCL_IO       GPIO_NUM_22
#define I2C_MASTER_SDA_IO       GPIO_NUM_23
#define I2C_MASTER_NUM          I2C_NUM_0
#define I2C_MASTER_FREQ_HZ      400000
#define I2C_MASTER_TIMEOUT_MS   1000

#define BME280_ADDRESS          0x77
#define BME280_ID_REG           0xD0

extern "C" void app_main(void) {
    logger.setLevel(LOGGER_LEVEL_DEBUG);
    logger.enableModuleDebug(LOGGER_MODULE_BME280_DRIVER);

    I2CMaster ic2Master;
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
        int32_t temperatureHundredthsC;
        uint32_t pressureHundredthsMBar;
        uint32_t relativeHumidityHundredths;
        bme280Driver.read(temperatureHundredthsC, pressureHundredthsMBar,
                          relativeHumidityHundredths);

        logger << logNotifyMain << "Temperature = " << temperatureHundredthsC / 100 << "."
               << temperatureHundredthsC % 100 << " C";
        logger << " Pressure = " << pressureHundredthsMBar / 100 << "."
               << pressureHundredthsMBar % 100 << " mBar";
        logger << " Relative Humidity = " << relativeHumidityHundredths / 100 << "."
               << relativeHumidityHundredths % 100 << "%" << eol;

        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
