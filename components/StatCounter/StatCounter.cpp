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

#include "StatCounter.h"

#include "DataModelLeaf.h"

#include "TimeConstants.h"

#include "Logger.h"

#include <stdint.h>

StatCounter::StatCounter() : count(0), lastIntervalCount(0) {
}

void StatCounter::increment() {
    count++;
}

StatCounter StatCounter::operator ++ (int) {
    count++;

    return *this;
}

void StatCounter::update(DataModelLeaf &countLeaf, DataModelLeaf &rateLeaf, uint32_t msElapsed) {
    countLeaf << count;

    uint32_t countInInterval;
    if (count < lastIntervalCount) {
        // Rollover occurred....wow that must have tacken forever
        countInInterval = 0xffffffff - lastIntervalCount + 1 + count;
    } else {
        countInInterval = count - lastIntervalCount;
    }

    const uint32_t eventsPerSecond = (countInInterval * msInSecond) / msElapsed;
    rateLeaf << eventsPerSecond;

    taskLogger() << logDebugStatsManager << "Harvested counter: " << count << " ms " << msElapsed
                 <<" " << eventsPerSecond << "/sec" << eol;

    lastIntervalCount = count;
}
