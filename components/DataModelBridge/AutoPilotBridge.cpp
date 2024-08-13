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

#include "AutoPilotBridge.h"
#include "InstrumentData.h"
#include "AutoPilotData.h"

#include "NMEARSAMessage.h"
#include "NMEAHDGMessage.h"

#include "DataModelNode.h"
#include "DataModelTenthsInt16Leaf.h"
#include "DataModelTenthsUInt16Leaf.h"

AutoPilotBridge::AutoPilotBridge(InstrumentData &instrumentData)
    : autoPilotData(instrumentData.autoPilotData()) {
}

void AutoPilotBridge::bridgeNMEAHDGMessage(const NMEAHDGMessage *message) {
    autoPilotData.beginUpdates();
    message->magneticSensorHeading.publish(autoPilotData.headingSensorLeaf);
    message->magneticDeviation.publish(autoPilotData.headingDeviationLeaf);
    message->magneticVariation.publish(autoPilotData.headingVariationLeaf);
    autoPilotData.endUpdates();
}

void AutoPilotBridge::bridgeNMEARSAMessage(const NMEARSAMessage *message) {
    autoPilotData.beginUpdates();
    if (message->starboardRudderSensorAngleValid) {
        message->starboardRudderSensorAngle.publish(autoPilotData.rudderStarboardLeaf);
    } else {
        autoPilotData.rudderStarboardLeaf.removeValue();
    }

    if (message->portRudderSensorAngleValid) {
        message->portRudderSensorAngle.publish(autoPilotData.rudderPortLeaf);
    } else {
        autoPilotData.rudderPortLeaf.removeValue();
    }
    autoPilotData.endUpdates();
}
