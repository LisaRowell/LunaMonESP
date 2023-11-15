/*
 * This file is part of LunaMon (https://github.com/LisaRowell/LunaMon)
 * Copyright (C) 2023 Lisa Rowell
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

#include "Logger.h"
#include "ESPError.h"
#include "Error.h"

#include "driver/i2c.h"
#include "esp_err.h"

#include <string.h>
#include <stdint.h>
#include <stddef.h>

#define I2C_MASTER_FREQ_HZ      400000
#define I2C_MASTER_TIMEOUT_MS   1000

I2CMaster::I2CMaster(i2c_port_t instance, gpio_num_t sclIOPin, gpio_num_t sdaIOPin)
    : instance(instance) {
    esp_err_t error;

    i2c_config_t i2c_config;
    memset(&i2c_config, 0, sizeof(i2c_config_t));
    i2c_config.mode = I2C_MODE_MASTER;
    i2c_config.sda_io_num = sdaIOPin;
    i2c_config.sda_pullup_en = GPIO_PULLUP_ENABLE;
    i2c_config.scl_io_num = sclIOPin;
    i2c_config.scl_pullup_en = GPIO_PULLUP_ENABLE;
    i2c_config.master.clk_speed = I2C_MASTER_FREQ_HZ;
    i2c_config.clk_flags = 0;
    
    error = i2c_param_config(instance, &i2c_config);
    if (error != ESP_OK) {
        logger << logErrorI2CMaster << "Failed to configure i2c driver: " << ESPError(error) << eol;
        errorExit();
    }
    
    error = i2c_driver_install(instance, I2C_MODE_MASTER, 0, 0, 0);
    if (error != ESP_OK) {
        logger << logErrorI2CMaster << "Failed to install i2c driver: " << ESPError(error) << eol;
        errorExit();
    }

    logger << logNotifyI2CMaster << "I2C Master Initialized." << eol;
}

uint8_t I2CMaster::readByte(uint8_t deviceAddr, uint8_t byteAddr) {
    uint8_t address = byteAddr;
    uint8_t value;
    esp_err_t error = i2c_master_write_read_device(instance, deviceAddr, &address, 1, &value, 1,
                                                   I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS); 
    if (error != ESP_OK) {
        throw error;
    }

    return value;
}

void I2CMaster::writeByte(uint8_t deviceAddr, uint8_t byteAddr, uint8_t value) {
    uint8_t write_buf[2] = { byteAddr, value };

    esp_err_t error = i2c_master_write_to_device(instance, deviceAddr, write_buf,
                                                 sizeof(write_buf),
                                                 I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS);
    if (error != ESP_OK) {
        throw error;
    }
}

void I2CMaster::readBytes(uint8_t deviceAddr, uint8_t address, uint8_t *bytes, size_t count) {
    esp_err_t error = i2c_master_write_read_device(instance, deviceAddr, &address, 1, bytes, count,
                                                   I2C_MASTER_TIMEOUT_MS / portTICK_PERIOD_MS); 
    if (error != ESP_OK) {
        throw error;
    }
}
