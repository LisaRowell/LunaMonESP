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

#ifndef TASK_OBJECTS_H
#define TASK_OBJECTS_H

#include "TaskObject.h"

#include "etl/intrusive_forward_list.h"

#include "freertos/FreeRTOS.h"
#include <freertos/semphr.h>

class TaskObjects {
    private:
        static constexpr uint32_t lockTimeoutMs = 60 * 1000;

        etl::intrusive_forward_list<TaskObject, tasksLink> taskObjects;
        SemaphoreHandle_t taskObjectsLock;

        void takeTaskObjectsLock();
        void releaseTaskObjectsLock();

    public:
        TaskObjects();
        void addTaskObject(TaskObject &taskObject);
        void removeTaskObject(TaskObject &taskObject);
        void logStackSizes();
};

extern TaskObjects taskObjects;

#endif // TASK_OBJECTS_H
