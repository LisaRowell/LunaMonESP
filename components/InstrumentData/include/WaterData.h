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

#ifndef WATER_DATA_H
#define WATER_DATA_H

#include "InstrumentGroup.h"

#include "DataModelNode.h"
#include "DataModelBoolLeaf.h"
#include "DataModelTenthsInt16Leaf.h"
#include "DataModelTenthsUInt16Leaf.h"

class DataModel;
class StatsManager;

class WaterData : public InstrumentGroup {
    private:
        DataModelNode waterNode;
        DataModelNode waterDepthNode;
        DataModelNode depthBelowSurfaceNode;
        DataModelNode depthBelowTransducerNode;
        DataModelNode depthBelowKeelNode;
        DataModelNode depthAlarmsNode;
        DataModelNode waterHeadingNode;
        DataModelNode waterSpeedNode;
        DataModelNode waterAverageSpeedNode;
        DataModelNode waterSpeedSecondSensorNode;
        DataModelNode waterTemperatureNode;

    public:
        DataModelTenthsUInt16Leaf depthBelowSurfaceFeetLeaf;
        DataModelTenthsUInt16Leaf depthBelowSurfaceMetersLeaf;
        DataModelTenthsUInt16Leaf depthBelowSurfaceFathomsLeaf;
        DataModelTenthsUInt16Leaf depthBelowTransducerFeetLeaf;
        DataModelTenthsUInt16Leaf depthBelowTransducerMetersLeaf;
        DataModelTenthsUInt16Leaf depthBelowTransducerFathomsLeaf;
        DataModelTenthsInt16Leaf depthBelowKeelFeetLeaf;
        DataModelTenthsInt16Leaf depthBelowKeelMetersLeaf;
        DataModelTenthsInt16Leaf depthBelowKeelFathomsLeaf;
        DataModelTenthsUInt16Leaf waterHeadingTrueLeaf;
        DataModelTenthsUInt16Leaf waterHeadingMagneticLeaf;
        DataModelTenthsInt16Leaf waterSpeedKnotsLeaf;
        DataModelTenthsInt16Leaf waterSpeedKMPHLeaf;
        DataModelTenthsInt16Leaf waterSpeedMPHLeaf;
        DataModelTenthsInt16Leaf waterAverageSpeedKnotsLeaf;
        DataModelTenthsInt16Leaf waterAverageSpeedMPHLeaf;
        DataModelBoolLeaf waterAverageSpeedStoppedLeaf;
        DataModelTenthsInt16Leaf waterSpeedSecondSensorKnotsLeaf;
        DataModelTenthsInt16Leaf waterSpeedSecondSensorMPHLeaf;
        DataModelTenthsInt16Leaf waterTemperatureCelsiusLeaf;
        DataModelTenthsInt16Leaf waterTemperatureFahrenheitLeaf;
        DataModelTenthsInt16Leaf waterTemperatureSensorDefectiveLeaf;
        DataModelBoolLeaf anchorDepthAlarmLeaf;
        DataModelBoolLeaf shallowDepthAlarmLeaf;
        DataModelBoolLeaf deepDepthAlarmLeaf;
        DataModelBoolLeaf depthTransducerDefectiveLeaf;

        WaterData(DataModel &dataModel, DataModelNode &instrumentDataNode,
                  StatsManager &statsManager);
};

#endif // WATER_DATA_H
