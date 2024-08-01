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

#include "NMEAAutoPilotBridge.h"

#include "NMEARSAMessage.h"

#include "DataModel.h"
#include "DataModelNode.h"
#include "DataModelTenthsInt16Leaf.h"

#include "StatCounter.h"

NMEAAutoPilotBridge::NMEAAutoPilotBridge(DataModel &dataModel, StatCounter &messagesBridgedCounter)
    : messagesBridgedCounter(messagesBridgedCounter),
      autoPilotNode("autoPilot", &dataModel.rootNode()),
      autoPilotRudderNode("rudder", &autoPilotNode),
      autopilotRudderStarboardLeaf("starboard", &autoPilotRudderNode),
      autopilotRudderPortLeaf("port", &autoPilotRudderNode) {
}

void NMEAAutoPilotBridge::bridgeNMEARSAMessage(const NMEARSAMessage *message) {
    if (message->starboardRudderSensorAngleValid) {
        message->starboardRudderSensorAngle.publish(autopilotRudderStarboardLeaf);
    } else {
        autopilotRudderStarboardLeaf.removeValue();
    }

    if (message->portRudderSensorAngleValid) {
        message->portRudderSensorAngle.publish(autopilotRudderPortLeaf);
    } else {
        autopilotRudderPortLeaf.removeValue();
    }

    messagesBridgedCounter++;
}
