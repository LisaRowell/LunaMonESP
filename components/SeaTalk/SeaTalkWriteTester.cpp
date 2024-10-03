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

#include "SeaTalkWriteTester.h"

#include "SeaTalkMaster.h"
#include "SeaTalkLampIntensity.h"

#include "TaskObject.h"
#include "Logger.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

SeaTalkWriteTester::SeaTalkWriteTester(SeaTalkMaster &master)
    : TaskObject("SeaTalk Write Tester", LOGGER_LEVEL_DEBUG, stackSize),
      master(master),
      testLampIntensity(SeaTalkLampIntensity::L0) {
}

void SeaTalkWriteTester::task() {
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(testIntervalms));

        testLampIntensity.cycle();
        logger << logDebugSeaTalk << "Sending SeaTalk command to change lamp intensity to "
               << testLampIntensity << eol;
        master.setLampIntensity(testLampIntensity);
    }
}
