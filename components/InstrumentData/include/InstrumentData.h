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

#ifndef INSTRUMENT_DATA_H
#define INSTRUMENT_DATA_H

#include "AutoPilotData.h"
#include "GPSData.h"
#include "WaterData.h"
#include "WindData.h"

#include "DataModelNode.h"

class DataModel;

class InstrumentData {
    private:
        DataModelNode sysNode;
        AutoPilotData _autoPilotData;
        GPSData _gpsData;
        WaterData _waterData;
        WindData _windData;

    public:
        InstrumentData(DataModel &dataModel, StatsManager &statsManager);
        AutoPilotData &autoPilotData();
        GPSData &gpsData();
        WaterData &waterData();
        WindData &windData();
};

#endif // INSTRUMENT_DATA_H
