/*
 * This file is part of LunaMon (https://github.com/LisaRowell/LunaMon)
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

#ifndef ENVIRONMENTAL_MON_H
#define ENVIRONMENTAL_MON_H

#include "DataModelNode.h"
#include "DataModelUInt8Leaf.h"
#include "TaskObject.h"
#include "BME280Driver.h"
#include "ENS160Driver.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

class DataModel;
class I2CMaster;
class StatusLED;

class EnvironmentalMon : public TaskObject {
    private:
        DataModelNode environmentDataModelNode;
        DataModelNode cabinDataModelNode;
        DataModelUInt8Leaf aqiDataModelLeaf;

        StatusLED *statusLED;
        bool bme280Functional;
        BME280Driver *bme280Driver;
        bool ens160Functional;
        ENS160Driver *ens160Driver;

        virtual void task() override;
        void detectBME280();
        void pollBME280();
        void detectENS160();
        void pollENS160();
        void statusLEDOn();
        void statusLEDOff();
        void statusNormalFlash();
        void statusErrorFlash();

    public:
        EnvironmentalMon(DataModel &dataModel, I2CMaster &ic2Master, StatusLED *statusLED);
};

#endif // ENVIRONMENTAL_MON_H
