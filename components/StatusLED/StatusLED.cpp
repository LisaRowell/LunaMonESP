/*
 * This file is part of LunaMon (https://github.com/LisaRowell/LunaMonESP)
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

#include "StatusLED.h"

#include "LED.h"
#include "TaskObject.h"
#include "Error.h"

#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define STACK_SIZE  (2 *1024)

StatusLED::StatusLED(gpio_num_t gpioPin)
    : LED(gpioPin),
      TaskObject("Status LED", LOGGER_LEVEL_DEBUG, STACK_SIZE),
      state(STATUS_LED_OFF) {
    start();
}

void StatusLED::task() {
    TickType_t cycleTicks = portMAX_DELAY;
    bool toggling = false;
    LED::off();

    while (true != false) {
        uint32_t notification;
        BaseType_t received = xTaskNotifyWait(0, ULONG_MAX, &notification, cycleTicks);
        if (received) {
            StatusLEDState newState = (StatusLEDState)notification;
            switch (newState) {
                case STATUS_LED_OFF:
                    LED::off();
                    cycleTicks = portMAX_DELAY;
                    toggling = false;
                    break;

                case STATUS_LED_NORMAL_FLASH:
                    LED::on();
                    cycleTicks = pdMS_TO_TICKS(1000);
                    toggling = true;
                    break;

                case STATUS_LED_ERROR_FLASH:
                    LED::on();
                    cycleTicks = pdMS_TO_TICKS(500);
                    toggling = true;
                    break;

                case STATUS_LED_ON:
                    LED::on();
                    cycleTicks = portMAX_DELAY;
                    toggling = false;
                    break;

                default:
                    fatalError("Bad Status LED state");
            }
        } else {
            if (toggling) {
                LED::toggle();
            }
        }
    }
}

void StatusLED::off() {
    changeState(STATUS_LED_OFF);
}

void StatusLED::errorFlash() {
    changeState(STATUS_LED_ERROR_FLASH);

}

void StatusLED::normalFlash() {
    changeState(STATUS_LED_NORMAL_FLASH);
}

void StatusLED::on() {
    changeState(STATUS_LED_ON);
}

void StatusLED::changeState(StatusLEDState newState) {
    state = newState;
    xTaskNotify(_task, state, eSetValueWithOverwrite);
}
