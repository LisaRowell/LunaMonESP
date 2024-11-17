/*
 * This file is part of LunaMon (https://github.com/LisaRowell/LunaMonESP)
 * Copyright (C) 2024 Lisa Rowell
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

#include "Buzzer.h"

#include "Logger.h"
#include "Error.h"

#include "driver/gpio.h"

Buzzer::Buzzer(gpio_num_t gpioPin, bool chirpAtStartup)
    : TaskObject("Buzzer", LOGGER_LEVEL_DEBUG, stackSize),
      gpioPin(gpioPin),
      chirpAtStartup(chirpAtStartup) {
}

void Buzzer::task() {
    initializeGPIO();

    if (chirpAtStartup) {
        vTaskDelay(pdMS_TO_TICKS(startupChirpDelayMS));
        on();
        vTaskDelay(pdMS_TO_TICKS(startupChirpDurationMS));
        off();
    }

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(10 * 1000));
    }
}

void Buzzer::initializeGPIO() {
    esp_err_t error;

    if ((error = gpio_set_direction(gpioPin, GPIO_MODE_OUTPUT)) != ESP_OK) {
        logger << logErrorBuzzer << "Failed to enable buzzer pin " << gpioPin << " as output"
               << eol;
        errorExit();
    }
    if ((error= gpio_set_level(gpioPin, 0)) != ESP_OK) {
        logger << logErrorBuzzer << "Failed to set initial state for buzzer pin " << gpioPin << eol;
        errorExit();
    }
}

void Buzzer::on() {
    esp_err_t error;

    logger << logDebugBuzzer << "Turning buzzer on" << eol;
    if ((error = gpio_set_level(gpioPin, 1)) != ESP_OK) {
        logger << logWarnBuzzer << "Failed to set buzzer pin " << gpioPin << " high" << eol;
    }
}

void Buzzer::off() {
    esp_err_t error;

    logger << logDebugBuzzer << "Turning buzzer off" << eol;
    if ((error = gpio_set_level(gpioPin, 0)) != ESP_OK) {
        logger << logWarnBuzzer << "Failed to set buzzer pin " << gpioPin << " low" << eol;
    }
}
