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

#include "LunaMon.h"

#include "StatusLED.h"
#include "I2CMaster.h"
#include "EnvironmentalMon.h"
#include "Logger.h"
#include "ESPError.h"
#include "Error.h"

#include "driver/gpio.h"
#include "driver/i2c.h"
#include "nvs_flash.h"
#include "esp_err.h"

#include "freertos/FreeRTOS.h"
#include "freertos/portmacro.h"

#define STATUS_LED_GPIO     ((gpio_num_t)CONFIG_STATUS_LED_GPIO)
#define I2C_MASTER_NUM      ((i2c_port_t)CONFIG_I2C_MASTER_NUM)
#define I2C_MASTER_SCL_IO   ((gpio_num_t)CONFIG_I2C_MASTER_SCL_IO)
#define I2C_MASTER_SDA_IO   ((gpio_num_t)CONFIG_I2C_MASTER_SDA_IO)

LunaMon::LunaMon() : statusLED(STATUS_LED_GPIO) {
    logger.setLevel(LOGGER_LEVEL_DEBUG);
//    logger.enableModuleDebug(LOGGER_MODULE_BME280_DRIVER);
//    logger.enableModuleDebug(LOGGER_MODULE_ENS160_DRIVER);
}

void LunaMon::run() {
    initNVS();

    wifiManager.start();

    I2CMaster ic2Master(I2C_MASTER_NUM, I2C_MASTER_SCL_IO, I2C_MASTER_SDA_IO);

    EnvironmentalMon environmentalMon(ic2Master, statusLED);

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(10000));
    }
}

void LunaMon::initNVS() {
    esp_err_t error;

    if ((error = nvs_flash_init()) != ESP_OK) {
        if (error == ESP_ERR_NVS_NO_FREE_PAGES || error == ESP_ERR_NVS_NEW_VERSION_FOUND) {
            logger << logNotifyMain << "Erasing flash" << eol;
            if ((error = nvs_flash_erase()) != ESP_OK) {
                logger << logNotifyMain << "Failed to erase flash: " << ESPError(error) << eol;
                errorExit();
            }

            if ((error = nvs_flash_init()) != ESP_OK) {
                logger << logNotifyMain << "Failed to initialize flash: " << ESPError(error) << eol;
                errorExit();
            }
        } else {
            logger << logNotifyMain << "Failed to initialize flash: " << ESPError(error) << eol;
            errorExit();
        }
    }
}
