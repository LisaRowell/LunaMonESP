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

#ifndef TASK_OBJECT_H
#define TASK_OBJECT_H

#include "Logger.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <stddef.h>

class TaskObject {
    public:
        typedef enum {
            LOW_PRIORITY = tskIDLE_PRIORITY,
            MEDIUM_PRIORITY = tskIDLE_PRIORITY + 1,
            HIGH_PRIORITY = tskIDLE_PRIORITY + 2
        } TaskPriority;

    private:
        const char *name;
        size_t stackSize;
        TaskPriority priority;

        static void startTask(void *task);

    protected:
        TaskHandle_t _task;
        // Each task needs it's own logger since it holds state as a message is built.
        Logger logger;

        virtual void task() = 0;

    public:
        TaskObject(const char *name, LoggerLevel level, size_t stackSize,
                   TaskPriority priority = MEDIUM_PRIORITY);
        void start();
        TaskHandle_t taskHandle();

        // Copy of a task object is a bad idea as theres a task running that would not be
        // duplicated. Prevent the automatic constructors that involve copies and moves.
        TaskObject(const TaskObject &) = delete;
        TaskObject(TaskObject &&) = delete;
        TaskObject& operator=(const TaskObject &) = delete;
        TaskObject& operator=(TaskObject &&) = delete;
};

#endif // TASK_OBJECT_H
