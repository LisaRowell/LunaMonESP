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

#include "Error.h"

#include <freertos/semphr.h>

#define STACK_SIZE  (8 * 1024)

DataModel::DataModel() : TaskObject("DataModel", LOGGER_LEVEL_DEBUG, STACK_SIZE) {
    if ((subscriptionLock = xSemaphoreCreateMutex()) == nullptr) {
        logger << logErrorMQTT << "Failed to create subscriptionLock mutex" << eol;
        errorExit();
    }
}

void DataModel::task() {
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(10000));

        // Later we'll export stats here. For dumping, a dump() could be done here.
    }
}

DataModelRoot &DataModel::rootNode() {
    return _rootNode;
}

bool DataModel::subscribe(const char *topicFilter, DataModelSubscriber &subscriber,
                          uint32_t cookie) {
    takeSubscriptionLock();
    bool result = _rootNode.subscribe(topicFilter, subscriber, cookie);
    releaseSubscriptionLock();

    return result;
}

void DataModel::unsubscribe(const char *topicFilter, DataModelSubscriber &subscriber) {
    takeSubscriptionLock();
    _rootNode.unsubscribe(topicFilter, subscriber);
    releaseSubscriptionLock();
}

void DataModel::unsubscribeAll(DataModelSubscriber &subscriber) {
    taskLogger() << logDebugDataModel << "Unsubscribing client '"<< subscriber.name()
                 << "' from all topics." << eol;
    takeSubscriptionLock();
    _rootNode.unsubscribeAll(subscriber);
    releaseSubscriptionLock();
}

void DataModel::takeSubscriptionLock() {
    if (xSemaphoreTake(subscriptionLock, pdMS_TO_TICKS(lockTimeoutMs)) != pdTRUE) {
        taskLogger() << logErrorDataModel << "Failed to get session lock mutex" << eol;
        errorExit();
    }
}

void DataModel::releaseSubscriptionLock() {
    xSemaphoreGive(subscriptionLock);
}

// Debuging method to dump out the data model tree. Useful debugging tree issues and verifying
// updates. Not called, but shouldn't be removed.
void DataModel::dump() {
    logger << logDebugDataModel << "Datamodel:" << eol;
    _rootNode.dump();
}

void DataModel::leafUpdated() {
}
