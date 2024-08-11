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

#include "InstrumentGroup.h"

#include "StatCounter.h"
#include "StatsManager.h"

#include "DataModel.h"
#include "DataModelNode.h"

#include "Logger.h"
#include "Error.h"

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

InstrumentGroup::InstrumentGroup(const char *name, const char *nodeName,
                                 DataModelNode &instrumentDataNode, StatsManager &statsManager)
    : name(name),
      updatesCounter(),
      groupSysNode(nodeName, &instrumentDataNode),
      updatesLeaf("updates", &groupSysNode),
      updateRateLeaf("updateRate", &groupSysNode) {
    statsManager.addStatsHolder(*this);

    if ((mutex = xSemaphoreCreateMutex()) == nullptr) {
        logger() << logErrorDataModelBridge << "Failed to create " << name <<" DeviceGroup mutex"
                 << eol;
        errorExit();
    }
}

void InstrumentGroup::beginUpdates() {
    if (xSemaphoreTake(mutex, pdMS_TO_TICKS(lockTimeoutMs)) != pdTRUE) {
        logger() << logErrorDataModelBridge << "Failed to take " << name << " DeviceGroup mutex"
                 << eol;
        errorExit();
    }
}

void InstrumentGroup::endUpdates() {
    updatesCounter++;
    xSemaphoreGive(mutex);
}

void InstrumentGroup::exportStats(uint32_t msElapsed) {
    updatesCounter.update(updatesLeaf, updateRateLeaf, msElapsed);
}
