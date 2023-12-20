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

#include "LED.h"

#include "driver/gpio.h"

LED::LED(gpio_num_t gpioPin) : gpioPin(gpioPin) {
    gpio_reset_pin(gpioPin);
    gpio_set_direction(gpioPin, GPIO_MODE_OUTPUT);

    gpio_set_level(gpioPin, 0);
    isOn = false;
}

void LED::on() {
    gpio_set_level(gpioPin, 1);
    isOn = true;
}

void LED::off() {
    gpio_set_level(gpioPin, 0);
    isOn = false;
}

void LED::toggle() {
    if (isOn) {
        off();
    } else {
        on();
    }
}
