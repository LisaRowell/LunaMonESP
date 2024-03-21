/*
 * This file is part of LunaMon (https://github.com/LisaRowell/LunaMonESP)
 * Copyright (C) 2021-2024 Lisa Rowell
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

#ifndef PASSIVE_TIMER_H
#define PASSIVE_TIMER_H

#include <stdint.h>

class PassiveTimer {
    private:
        const uint32_t halfMilliTimerRange = 0x80000000;
        uint32_t endTime;

        uint32_t millis() const;

    public:
        void setMilliSeconds(uint32_t milliSeconds);
        void setSeconds(uint32_t seconds);
        void setNow();
        void advanceMilliSeconds(uint32_t milliSeconds);
        void advanceSeconds(uint32_t seconds);
        bool expired();
        uint32_t timeInMilliSeconds();
        uint32_t elapsedTime();
};

#endif
