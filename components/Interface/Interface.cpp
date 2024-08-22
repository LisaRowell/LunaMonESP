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

#include "Interface.h"
#include "InterfaceProtocol.h"

#include "StatCounter.h"
#include "StatsManager.h"

#include "DataModel.h"
#include "DataModelNode.h"
#include "TaskObject.h"
#include "Error.h"

#include "etl/string.h"

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

#include <stddef.h>

Interface::Interface(const char *name, enum InterfaceProtocol protocol, StatsManager &statsManager,
                     DataModel &dataModel, size_t stackSize)
    : TaskObject(name, LOGGER_LEVEL_DEBUG, stackSize),
      _name(name),
      protocol(protocol),
      _interfaceNode(name, &dataModel.sysNode()),
      receivedBytesLeaf("receivedBytes", &_interfaceNode),
      receivedByteRateLeaf("receivedByteRate", &_interfaceNode),
      receivedBytes() {
    statsManager.addStatsHolder(*this);

    if ((writeLock = xSemaphoreCreateMutex()) == nullptr) {
        fatalError("Failed to create Interface write mutex");
    }
}

DataModelNode &Interface::interfaceNode() {
    return _interfaceNode;
}

const char *Interface::name() const {
    return _name;
}

size_t Interface::send(const char *string) {
    size_t length = strlen(string);
    return sendBytes(string, length);
}

size_t Interface::send(const etl::istring &string) {
    size_t length = string.length();
    return sendBytes(string.data(), length);
}

void Interface::takeWriteLock() {
    if (xSemaphoreTake(writeLock, pdMS_TO_TICKS(lockTimeoutMs)) != pdTRUE) {
        fatalError("Failed to get contacts lock mutex");
    }
}

void Interface::releaseWriteLock() {
    xSemaphoreGive(writeLock);
}

void Interface::exportStats(uint32_t msElapsed) {
    receivedBytes.update(receivedBytesLeaf, receivedByteRateLeaf, msElapsed);
}
