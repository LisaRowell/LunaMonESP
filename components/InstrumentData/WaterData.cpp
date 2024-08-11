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

#include "WaterData.h"
#include "InstrumentGroup.h"

#include "DataModel.h"
#include "DataModelNode.h"
#include "DataModelBoolLeaf.h"
#include "DataModelTenthsInt16Leaf.h"
#include "DataModelTenthsUInt16Leaf.h"

WaterData::WaterData(DataModel &dataModel, DataModelNode &instrumentDataNode,
                     StatsManager &statsManager)
    : InstrumentGroup("Water", "water", instrumentDataNode, statsManager),
      waterNode("water", &dataModel.rootNode()),
      waterDepthNode("depth", &waterNode),
      depthBelowSurfaceNode("belowSurface", &waterDepthNode),
      depthBelowTransducerNode("belowTransducer", &waterDepthNode),
      depthBelowKeelNode("belowKeel", &waterDepthNode),
      depthAlarmsNode("alarms", &waterDepthNode),
      waterHeadingNode("heading", &waterNode),
      waterSpeedNode("speed", &waterNode),
      waterAverageSpeedNode("average", &waterSpeedNode),
      waterSpeedSecondSensorNode("secondSensor", &waterSpeedNode),
      waterTemperatureNode("temperature", &waterNode),
      depthBelowSurfaceFeetLeaf("feet", &depthBelowSurfaceNode),
      depthBelowSurfaceMetersLeaf("meters", &depthBelowSurfaceNode),
      depthBelowSurfaceFathomsLeaf("fathoms", &depthBelowSurfaceNode),
      depthBelowTransducerFeetLeaf("feet", &depthBelowTransducerNode),
      depthBelowTransducerMetersLeaf("meters", &depthBelowTransducerNode),
      depthBelowTransducerFathomsLeaf("fathoms", &depthBelowTransducerNode),
      depthBelowKeelFeetLeaf("feet", &depthBelowKeelNode),
      depthBelowKeelMetersLeaf("meters", &depthBelowKeelNode),
      depthBelowKeelFathomsLeaf("fathoms", &depthBelowKeelNode),
      waterHeadingTrueLeaf("true", &waterHeadingNode),
      waterHeadingMagneticLeaf("magnetic", &waterHeadingNode),
      waterSpeedKnotsLeaf("knots", &waterSpeedNode),
      waterSpeedKMPHLeaf("kmph", &waterSpeedNode),
      waterSpeedMPHLeaf("mph", &waterSpeedNode),
      waterAverageSpeedKnotsLeaf("knots", &waterAverageSpeedNode),
      waterAverageSpeedMPHLeaf("mph", &waterAverageSpeedNode),
      waterAverageSpeedStoppedLeaf("stopped", &waterAverageSpeedNode),
      waterSpeedSecondSensorKnotsLeaf("knots", &waterSpeedSecondSensorNode),
      waterSpeedSecondSensorMPHLeaf("mph", &waterSpeedSecondSensorNode),
      waterTemperatureCelsiusLeaf("celsius", &waterTemperatureNode),
      waterTemperatureFahrenheitLeaf("fahrenheit", &waterTemperatureNode),
      waterTemperatureSensorDefectiveLeaf("sensorDefective", &waterTemperatureNode),
      anchorDepthAlarmLeaf("anchor", &depthAlarmsNode),
      shallowDepthAlarmLeaf("shallow", &depthAlarmsNode),
      deepDepthAlarmLeaf("deep", &depthAlarmsNode),
      depthTransducerDefectiveLeaf("transducerDefective", &waterDepthNode) {
}
