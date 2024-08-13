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

#include "AutoPilotData.h"
#include "InstrumentGroup.h"

#include "DataModel.h"
#include "DataModelNode.h"
#include "DataModelBoolLeaf.h"
#include "DataModelUInt8Leaf.h"
#include "DataModelStringLeaf.h"
#include "DataModelTenthsInt16Leaf.h"
#include "DataModelTenthsUInt16Leaf.h"

#include "etl/string.h"

AutoPilotData::AutoPilotData(DataModel &dataModel, DataModelNode &instrumentDataNode,
                             StatsManager &statsManager)
    : InstrumentGroup("AutoPilot", "autoPilot", instrumentDataNode, statsManager),
      autoPilotNode("autoPilot", &dataModel.rootNode()),
      headingNode("heading", &autoPilotNode),
      rudderNode("rudder", &autoPilotNode),
      alarmNode("alarms", &autoPilotNode),
      statusLeaf("status", &autoPilotNode, statusBuffer),
      modeLeaf("mode", &autoPilotNode, modeBuffer),
      headingSensorLeaf("sensor", &headingNode),
      headingDeviationLeaf("deviation", &headingNode),
      headingVariationLeaf("variation", &headingNode),
      courseLeaf("course", &autoPilotNode),
      rudderStarboardLeaf("starboard", &rudderNode),
      rudderCenterLeaf("center", &rudderNode),
      rudderPortLeaf("port", &rudderNode),
      offCourseAlarmLeaf("offCourse", &alarmNode),
      windShiftAlarmLeaf("windShift", &alarmNode),
      knownDevicesLeaf("knownDevices", &dataModel.rootNode(), knownDevicesBuffer) {
}
