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
#include "InstrumentData.h"
#include "WaterData.h"

#include "NMEADBKMessage.h"
#include "NMEADBSMessage.h"
#include "NMEADBTMessage.h"
#include "NMEADPTMessage.h"
#include "NMEAMTWMessage.h"
#include "NMEAVHWMessage.h"
#include "NMEATemperatureUnits.h"
#include "NMEATenthsInt16.h"

#include "DataModelNode.h"
#include "DataModelTenthsInt16Leaf.h"
#include "DataModelTenthsUInt16Leaf.h"

#include "TenthsInt16.h"

WaterBridge::WaterBridge(InstrumentData &instrumentData)
    : waterData(instrumentData.waterData()) {
}

void WaterBridge::bridgeNMEADBKMessage(const NMEADBKMessage *message) {
    waterData.beginUpdates();
    message->depthFeet.publish(waterData.depthBelowKeelFeetLeaf);
    message->depthMeters.publish(waterData.depthBelowKeelMetersLeaf);
    message->depthFathoms.publish(waterData.depthBelowKeelFathomsLeaf);
    waterData.endUpdates();
}

void WaterBridge::bridgeNMEADBSMessage(const NMEADBSMessage *message) {
    waterData.beginUpdates();
    message->depthFeet.publish(waterData.depthBelowSurfaceFeetLeaf);
    message->depthMeters.publish(waterData.depthBelowSurfaceMetersLeaf);
    message->depthFathoms.publish(waterData.depthBelowSurfaceFathomsLeaf);
    waterData.endUpdates();
}

void WaterBridge::bridgeNMEADBTMessage(const NMEADBTMessage *message) {
    waterData.beginUpdates();
    message->depthFeet.publish(waterData.depthBelowTransducerFeetLeaf);
    message->depthMeters.publish(waterData.depthBelowTransducerMetersLeaf);
    message->depthFathoms.publish(waterData.depthBelowTransducerFathomsLeaf);
    waterData.endUpdates();
}

void WaterBridge::bridgeNMEADPTMessage(const NMEADPTMessage *message) {
    waterData.beginUpdates();
    message->depthBelowTransducerMeters.publish(waterData.depthBelowTransducerMetersLeaf);

    // The transducer offset field in this message is a little wonky as it can indicate either
    // a distance from the transducer to the keel or a distance from the transducer to the water
    // line, indicated by the sign.
    TenthsUInt16 depthBelowTransducerMeters = message->depthBelowTransducerMeters;
    TenthsInt16 transducerOffsetMeters = message->transducerOffsetMeters;
    if (transducerOffsetMeters < 0) {
        TenthsInt16 depthBelowKeelMeters;
        depthBelowKeelMeters = depthBelowTransducerMeters - transducerOffsetMeters.abs();
        waterData.depthBelowKeelMetersLeaf = depthBelowKeelMeters;
    } else {
        TenthsUInt16 depthBelowSurfaceMeters;
        depthBelowSurfaceMeters = depthBelowTransducerMeters + transducerOffsetMeters.abs();
        waterData.depthBelowSurfaceMetersLeaf = depthBelowSurfaceMeters;
    }
    waterData.endUpdates();
}

void WaterBridge::bridgeNMEAMTWMessage(const NMEAMTWMessage *message) {
    waterData.beginUpdates();
    switch (message->waterTemperatureUnits) {
        case NMEATemperatureUnits::CELSIUS:
            message->waterTemperature.publish(waterData.waterTemperatureCelsiusLeaf);
            break;

        case NMEATemperatureUnits::FAHRENHEIT:
            message->waterTemperature.publish(waterData.waterTemperatureFahrenheitLeaf);
            break;

        default:
            logger() << logWarnDataModelBridge << "Unhandled water temperature units ("
                     << message->waterTemperatureUnits << ") in NMEA MTW message" << eol;
    }
    waterData.endUpdates();
}

void WaterBridge::bridgeNMEAVHWMessage(const NMEAVHWMessage *message) {
    waterData.beginUpdates();
    message->waterHeadingTrue.publish(waterData.waterHeadingTrueLeaf);
    message->waterHeadingMagnetic.publish(waterData.waterHeadingMagneticLeaf);
    message->waterSpeedKnots.publish(waterData.waterSpeedKnotsLeaf);
    message->waterSpeedKMPH.publish(waterData.waterSpeedKMPHLeaf);
    waterData.endUpdates();
}
