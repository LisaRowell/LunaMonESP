/*
 * This file is part of LunaMon (https://github.com/LisaRowell/LunaMonESP)
 * Copyright (C) 2023 Lisa Rowell
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

#include "TaskObject.h"

#include "Logger.h"
#include "Error.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <stddef.h>

TaskObject::TaskObject(const char *name, LoggerLevel level, size_t stackSize, TaskPriority priority)
    : name(name),
      stackSize(stackSize),
      priority(priority),
      _task(nullptr),
      logger(level) {
}

void TaskObject::start() {
    BaseType_t created = xTaskCreate(startTask, name, stackSize, this,
                                     priority, &_task);
    if (created != pdPASS || _task == nullptr) {
        logger << logErrorTaskObject << "Failed to create " << name << " task" << eol;
        errorExit();
    }
}

TaskHandle_t TaskObject::taskHandle() {
    return _task;
}

void TaskObject::startTask(void *taskPtr) {
    TaskObject *task = (TaskObject *)taskPtr;

    Logger &logger = task->logger;
    logger.initForTask();

    logger << logNotifyTaskObject << "Starting task " << task->name << eol;

    task->task();

    logger << logErrorTaskObject << "Task " << task->name << " returned. Deleting task..." << eol;
    vTaskDelete(nullptr);
}
