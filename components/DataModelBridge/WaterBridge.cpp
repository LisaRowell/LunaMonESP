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

#include "WaterBridge.h"

#include "NMEADBKMessage.h"
#include "NMEADBSMessage.h"
#include "NMEADBTMessage.h"
#include "NMEADPTMessage.h"
#include "NMEAMTWMessage.h"
#include "NMEAVHWMessage.h"
#include "NMEATemperatureUnits.h"
#include "NMEATenthsInt16.h"

#include "DataModel.h"
#include "DataModelNode.h"
#include "DataModelTenthsInt16Leaf.h"
#include "DataModelTenthsUInt16Leaf.h"

#include "TenthsInt16.h"

#include "StatCounter.h"

WaterBridge::WaterBridge(DataModel &dataModel, StatCounter &messagesBridgedCounter)
    : messagesBridgedCounter(messagesBridgedCounter),
      waterNode("water", &dataModel.rootNode()),
      waterDepthNode("depth", &waterNode),
      depthBelowSurfaceNode("belowSurface", &waterDepthNode),
      depthBelowSurfaceFeetLeaf("feet", &depthBelowSurfaceNode),
      depthBelowSurfaceMetersLeaf("meters", &depthBelowSurfaceNode),
      depthBelowSurfaceFathomsLeaf("fathoms", &depthBelowSurfaceNode),
      depthBelowTransducerNode("belowTransducer", &waterDepthNode),
      depthBelowTransducerFeetLeaf("feet", &depthBelowTransducerNode),
      depthBelowTransducerMetersLeaf("meters", &depthBelowTransducerNode),
      depthBelowTransducerFathomsLeaf("fathoms", &depthBelowTransducerNode),
      depthBelowKeelNode("belowKeel", &waterDepthNode),
      depthBelowKeelFeetLeaf("feet", &depthBelowKeelNode),
      depthBelowKeelMetersLeaf("meters", &depthBelowKeelNode),
      depthBelowKeelFathomsLeaf("fathoms", &depthBelowKeelNode),
      waterHeadingNode("heading", &waterNode),
      waterHeadingTrueLeaf("true", &waterHeadingNode),
      waterHeadingMagneticLeaf("magnetic", &waterHeadingNode),
      waterSpeedNode("speed", &waterNode),
      waterSpeedKnotsLeaf("knots", &waterSpeedNode),
      waterSpeedKMPHLeaf("kmph", &waterSpeedNode),
      waterTemperatureNode("temperature", &waterNode),
      waterTemperatureCelsiusLeaf("celsius", &waterTemperatureNode),
      waterTemperatureFahrenheitLeaf("fahrenheit", &waterTemperatureNode) {
}

void WaterBridge::bridgeNMEADBKMessage(const NMEADBKMessage *message) {
    message->depthFeet.publish(depthBelowKeelFeetLeaf);
    message->depthMeters.publish(depthBelowKeelMetersLeaf);
    message->depthFathoms.publish(depthBelowKeelFathomsLeaf);

    messagesBridgedCounter++;
}

void WaterBridge::bridgeNMEADBSMessage(const NMEADBSMessage *message) {
    message->depthFeet.publish(depthBelowSurfaceFeetLeaf);
    message->depthMeters.publish(depthBelowSurfaceMetersLeaf);
    message->depthFathoms.publish(depthBelowSurfaceFathomsLeaf);

    messagesBridgedCounter++;
}

void WaterBridge::bridgeNMEADBTMessage(const NMEADBTMessage *message) {
    message->depthFeet.publish(depthBelowTransducerFeetLeaf);
    message->depthMeters.publish(depthBelowTransducerMetersLeaf);
    message->depthFathoms.publish(depthBelowTransducerFathomsLeaf);

    messagesBridgedCounter++;
}

void WaterBridge::bridgeNMEADPTMessage(const NMEADPTMessage *message) {
    message->depthBelowTransducerMeters.publish(depthBelowTransducerMetersLeaf);

    // The transducer offset field in this message is a little wonky as it can indicate either
    // a distance from the transducer to the keel or a distance from the transducer to the water
    // line, indicated by the sign.
    TenthsUInt16 depthBelowTransducerMeters = message->depthBelowTransducerMeters;
    TenthsInt16 transducerOffsetMeters = message->transducerOffsetMeters;
    if (transducerOffsetMeters < 0) {
        TenthsInt16 depthBelowKeelMeters;
        depthBelowKeelMeters = depthBelowTransducerMeters - transducerOffsetMeters.abs();
        depthBelowKeelMetersLeaf = depthBelowKeelMeters;
    } else {
        TenthsUInt16 depthBelowSurfaceMeters;
        depthBelowSurfaceMeters = depthBelowTransducerMeters + transducerOffsetMeters.abs();
        depthBelowSurfaceMetersLeaf = depthBelowSurfaceMeters;
    }

    messagesBridgedCounter++;
}

void WaterBridge::bridgeNMEAMTWMessage(const NMEAMTWMessage *message) {
    switch (message->waterTemperatureUnits) {
        case NMEATemperatureUnits::CELSIUS:
            message->waterTemperature.publish(waterTemperatureCelsiusLeaf);
            break;

        case NMEATemperatureUnits::FAHRENHEIT:
            message->waterTemperature.publish(waterTemperatureFahrenheitLeaf);
            break;

        default:
            logger() << logWarnDataModelBridge << "Unhandled water temperature units ("
                     << message->waterTemperatureUnits << ") in NMEA MTW message" << eol;
    }

    messagesBridgedCounter++;
}

void WaterBridge::bridgeNMEAVHWMessage(const NMEAVHWMessage *message) {
    message->waterHeadingTrue.publish(waterHeadingTrueLeaf);
    message->waterHeadingMagnetic.publish(waterHeadingMagneticLeaf);
    message->waterSpeedKnots.publish(waterSpeedKnotsLeaf);
    message->waterSpeedKMPH.publish(waterSpeedKMPHLeaf);

    messagesBridgedCounter++;
}
