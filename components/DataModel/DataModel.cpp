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

#include "DataModel.h"

#include "TaskObject.h"
#include "Logger.h"

#define STACK_SIZE  (8 * 1024)

DataModel::DataModel() : TaskObject("DataModel", LOGGER_LEVEL_DEBUG, STACK_SIZE) {
}

void DataModel::task() {
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(10000));

        dump();
    }
}

DataModelRoot &DataModel::rootNode() {
    return _rootNode;
}

// Debuging method to dump out the data model tree. Useful debugging tree issues and verifying
// updates. Not called, but shouldn't be removed.
void DataModel::dump() {
    logger << logDebugDataModel << "Datamodel:" << eol;
    _rootNode.dump();
}

void DataModel::leafUpdated() {
}
