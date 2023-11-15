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

#ifndef STATUS_LED_H
#define STATUS_LED_H

#include "LED.h"
#include "TaskObject.h"

enum StatusLEDState {
    STATUS_LED_OFF,
    STATUS_LED_ERROR_FLASH,
    STATUS_LED_NORMAL_FLASH,
    STATUS_LED_ON,
};

class StatusLED : LED, TaskObject {
    private:
        StatusLEDState state;

        virtual void task() override;
        void changeState(StatusLEDState newState);

    public:
        StatusLED(gpio_num_t gpioPin);
        void off();
        void errorFlash();
        void normalFlash();
        void on();
};

#endif // STATUS_LED_H
