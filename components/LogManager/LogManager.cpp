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

#include "LogManager.h"

#include "DataModel.h"
#include "DataModelNode.h"

LogManager::LogManager(DataModel &dataModel) :
    logNode("log", &dataModel.sysNode()),
    log1Leaf("1", &logNode, log1Buffer),
    log2Leaf("2", &logNode, log2Buffer),
    log3Leaf("3", &logNode, log3Buffer),
    log4Leaf("4", &logNode, log4Buffer),
    log5Leaf("5", &logNode, log5Buffer) {
}

// Later this will need to be extended to periodically grab important log messages and update
// the associated leafs.