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

#include "StatsManager.h"
#include "StatsHolder.h"

#include "PassiveTimer.h"

#include "Logger.h"

#include <etl/vector.h>

#include <stdint.h>

StatsManager::StatsManager()
    : TaskObject("Stats Manager", LOGGER_LEVEL_DEBUG, stackSize),
      statsHolders() {
    lastHarvestTime.setNow();
}

void StatsManager::addStatsHolder(StatsHolder &statsHolder) {
    statsHolders.push_back(statsHolder);
}

void StatsManager::task() {
    logger << logDebugStatsManager << "Starting StatsManager task..." << eol;

    while (1) {
        vTaskDelay(pdMS_TO_TICKS(statsUpdateTimeIntervalMs));

        const uint32_t elapsedTime = lastHarvestTime.elapsedTime();
        lastHarvestTime.setNow();

        logger << logDebugStatsManager << "Harvesting stats with elapsed time " << elapsedTime
               << "ms" << eol;

        for (StatsHolder &statsHolder : statsHolders) {
            statsHolder.exportStats(elapsedTime);
        }
    }
}
