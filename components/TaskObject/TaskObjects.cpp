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

#include "TaskObjects.h"
#include "TaskObject.h"

#include "Error.h"

#include "etl/intrusive_forward_list.h"

#include "freertos/FreeRTOS.h"
#include <freertos/semphr.h>

TaskObjects::TaskObjects() {
    if ((taskObjectsLock = xSemaphoreCreateMutex()) == nullptr) {
        fatalError("Failed to create taskObjectsLock mutex");
    }
}

void TaskObjects::addTaskObject(TaskObject &taskObject) {
    takeTaskObjectsLock();

    taskObjects.push_front(taskObject);

    releaseTaskObjectsLock();
}

void TaskObjects::removeTaskObject(TaskObject &taskObject) {
    takeTaskObjectsLock();

    taskObjects.remove(taskObject);

    releaseTaskObjectsLock();
}

void TaskObjects::logStackSizes() {
    // Normally it wouldn't be a great move to hold a lock while logging debug messages, but since
    // LunaMon doesn't have dynamically coming and going threads, this is fine. The lock probably
    // not even needed as by the time this is called all the threads that are going to be created
    // are already started. We do it anyway for future unforseen changes.
    takeTaskObjectsLock();

    for (TaskObject &task : taskObjects) {
        task.logStackSize();
    }

    releaseTaskObjectsLock();
}

void TaskObjects::takeTaskObjectsLock() {
    if (xSemaphoreTake(taskObjectsLock, pdMS_TO_TICKS(lockTimeoutMs)) != pdTRUE) {
        fatalError("Failed to take taskObjectsLock mutex");
    }
}

void TaskObjects::releaseTaskObjectsLock() {
    xSemaphoreGive(taskObjectsLock);
}

TaskObjects taskObjects;
