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

#ifndef STAT_COUNTER_H
#define STAT_COUNTER_H

class DataModelUInt32Leaf;

#include <stdint.h>

class StatCounter {
    private:
        uint32_t count;
        uint32_t lastIntervalCount;

    public:
        StatCounter();
        void increment();
        void incrementBy(uint32_t addition);
        StatCounter operator ++ (int);

        void update(DataModelUInt32Leaf &countLeaf, DataModelUInt32Leaf &rateLeaf,
                    uint32_t msElapsed);
};

#endif // STAT_COUNTER_H
