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

#include "StatusLED.h"

#include "LED.h"
#include "Error.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define STACK_SIZE  1024

void StatusLED::flashTask(void *pvParameters) {
    LED *led = (LED *)pvParameters;
    TickType_t cycleTicks = portMAX_DELAY;
    bool toggle = false;
    led->off();

    while (true != false) {
        uint32_t notification;
        BaseType_t received = xTaskNotifyWait(0, ULONG_MAX, &notification, cycleTicks);
        if (received) {
            StatusLEDState newState = (StatusLEDState)notification;
            switch (newState) {
                case STATUS_LED_OFF:
                    led->off();
                    cycleTicks = portMAX_DELAY;
                    toggle = false;
                    break;

                case STATUS_LED_NORMAL_FLASH:
                    led->on();
                    cycleTicks = pdMS_TO_TICKS(1000);
                    toggle = true;
                    break;

                case STATUS_LED_ERROR_FLASH:
                    led->on();
                    cycleTicks = pdMS_TO_TICKS(500);
                    toggle = true;
                    break;

                case STATUS_LED_ON:
                    led->on();
                    cycleTicks = portMAX_DELAY;
                    toggle = false;
                    break;

                default:
                    fatalError("Bad Status LED state");
            }
        } else {
            if (toggle) {
                led->toggle();
            }
        }
    }
}

StatusLED::StatusLED(gpio_num_t gpioPin) : LED(gpioPin), state(STATUS_LED_OFF) {
    BaseType_t created = xTaskCreate(flashTask, "StatusLEDFlashTask", STACK_SIZE, this,
                                     tskIDLE_PRIORITY, &flasherTask);
    if (created != pdPASS || flasherTask == NULL) {
        fatalError("Failed to create StatusLED task");
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
    xTaskNotify(flasherTask, state, eSetValueWithOverwrite);
}
