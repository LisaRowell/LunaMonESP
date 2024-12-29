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

#ifndef STATS_MANAGER_H
#define STATS_MANAGER_H

#include "TaskObject.h"

#include "StatsHolder.h"

#include "PassiveTimer.h"

#include "etl/intrusive_list.h"

#include <stddef.h>
#include <stdint.h>

class StatsHolder;

class StatsManager : public TaskObject {
    private:
        static constexpr size_t stackSize = 3 * 1024;
        static constexpr uint32_t statsUpdateTimeIntervalMs = 10 * 1000;

        PassiveTimer lastHarvestTime;
        etl::intrusive_list<StatsHolder, statsHolderLink> statsHolders;

        virtual void task() override;

    public:
        StatsManager();
        void addStatsHolder(StatsHolder &statsHolder);
};

#endif
