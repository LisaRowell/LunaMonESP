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

#ifndef LOG_MANAGER_H
#define LOG_MANAGER_H

#include "DataModelNode.h"
#include "DataModelStringLeaf.h"

#include "etl/string.h"

#include <stddef.h>

class DataModel;

class LogManager {
    private:
        static constexpr size_t maxLogEntryLength = 100;

        DataModelNode logNode;
        etl::string<maxLogEntryLength> log1Buffer;
        DataModelStringLeaf log1Leaf;
        etl::string<maxLogEntryLength> log2Buffer;
        DataModelStringLeaf log2Leaf;
        etl::string<maxLogEntryLength> log3Buffer;
        DataModelStringLeaf log3Leaf;
        etl::string<maxLogEntryLength> log4Buffer;
        DataModelStringLeaf log4Leaf;
        etl::string<maxLogEntryLength> log5Buffer;
        DataModelStringLeaf log5Leaf;
    
    public:
        LogManager(DataModel &dataModel);
};

#endif
