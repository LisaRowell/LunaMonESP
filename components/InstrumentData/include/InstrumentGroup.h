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

#ifndef INSTRUMENT_GROUP_H
#define INSTRUMENT_GROUP_H

#include "StatsHolder.h"
#include "StatCounter.h"

#include "DataModelNode.h"
#include "DataModelUInt32Leaf.h"

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

#include <stdint.h>

class StatsManager;

class InstrumentGroup : StatsHolder {
    private:
        static constexpr uint32_t lockTimeoutMs = 60 * 1000;

        const char *name;
        SemaphoreHandle_t mutex;
        StatCounter updatesCounter;
        DataModelNode groupSysNode;
        DataModelUInt32Leaf updatesLeaf;
        DataModelUInt32Leaf updateRateLeaf;

        virtual void exportStats(uint32_t msElapsed) override;

        protected:
            DataModelNode &groupNode();

    public:
        InstrumentGroup(const char *name, const char *nodeName, DataModelNode &instrumentDataNode,
                        StatsManager &statsManager);
        void beginUpdates();
        void endUpdates();
};

#endif // INSTRUMENT_GROUP_H
