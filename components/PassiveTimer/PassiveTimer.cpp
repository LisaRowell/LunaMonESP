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

#include "PassiveTimer.h"
#include "TimeConstants.h"

#include "Error.h"

#include "esp_timer.h"

#include <stdint.h>

void PassiveTimer::setMilliSeconds(uint32_t milliSeconds) {
    // The algorithm we use for detection of a timer end is designed for system uptimes greater than
    // the 50 days that it takes to wrap around a 32 bit millisecond counter and is based on seeing
    // if the end time is first half of the 2^32 period and the current time is in the later.
    // Because of this, we can't accept a timer period of greater than 25 days, give or take. This
    // should not present a hardship for the average coder.
    if (milliSeconds >= halfMilliTimerRange) {
        fatalError("Attempting to set a timer greater than 25 days.");
    }

    // This may wrap, but that's by design.
    endTime = millis() + milliSeconds;
}

void PassiveTimer::setSeconds(uint32_t seconds) {
    setMilliSeconds(seconds * msInSecond);
}

void PassiveTimer::setNow() {
    setMilliSeconds(0);
}

void PassiveTimer::advanceMilliSeconds(uint32_t milliSeconds) {
    // The algorithm we use for detection of a timer end is designed for system uptimes greater than
    // the 50 days that it takes to wrap around a 32 bit millisecond counter and is based on seeing
    // if the end time is first half of the 2^32 period and the current time is in the later.
    // Because of this, we can't accept a timer period of greater than 25 days, give or take. This
    // should not present a hardship for the average coder.
    if (milliSeconds >= halfMilliTimerRange) {
        fatalError("Attempting to advance a timer greater than 25 days.");
    }

    // This may wrap, but that's by design.
    endTime += milliSeconds;
}

void PassiveTimer::advanceSeconds(uint32_t seconds) {
    advanceMilliSeconds(seconds * msInSecond);
}

bool PassiveTimer::expired() {
    const uint32_t time = millis();

    // We deal with timers wrapping around the 32 bit millisecond timer by looking for the end time
    // being less than the mid point while the current time is after it. Since we limit the timer
    // interval to 2^31, this means that the millis counter needs to wrap around for the timer to
    // expire.
    if (time >= halfMilliTimerRange && endTime < halfMilliTimerRange) {
        return false;
    } else {
        return time >= endTime;
    }
}

uint32_t PassiveTimer::timeInMilliSeconds() {
    return endTime;
}

uint32_t PassiveTimer::elapsedTime() {
    uint32_t now = millis();

    if (now >= endTime) {
        return now - endTime;
    } else {
        return 0xffffffff - endTime + 1 + now;
    }
}

uint32_t PassiveTimer::millis() const {
    return (uint32_t)(esp_timer_get_time() / 1000);
}
