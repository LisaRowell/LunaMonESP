/*
 * This file is part of LunaMon (https://github.com/LisaRowell/LunaMonESP)
 * Copyright (C) 2021-2024 Lisa Rowell
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

#include "GPSBridge.h"
#include "InstrumentData.h"
#include "GPSData.h"

#include "NMEAGGAMessage.h"
#include "NMEAGLLMessage.h"
#include "NMEAGSAMessage.h"
#include "NMEAGSTMessage.h"
#include "NMEARMCMessage.h"
#include "NMEAVTGMessage.h"

#include "DataModelBoolLeaf.h"
#include "DataModelUInt16Leaf.h"
#include "DataModelTenthsInt16Leaf.h"
#include "DataModelTenthsUInt16Leaf.h"
#include "DataModelHundredthsUInt8Leaf.h"
#include "DataModelHundredthsUInt16Leaf.h"
#include "DataModelStringLeaf.h"

#include "etl/string.h"
#include "etl/string_stream.h"

GPSBridge::GPSBridge(InstrumentData &instrumentData)
    : gpsData(instrumentData.gpsData()) {
}

void GPSBridge::bridgeNMEAGGAMessage(const NMEAGGAMessage *message) {
    gpsData.beginUpdates();
    message->time.publish(gpsData.gpsTimeLeaf);
    message->latitude.publish(gpsData.gpsLatitudeLeaf);
    message->longitude.publish(gpsData.gpsLongitudeLeaf);
    message->gpsQuality.publish(gpsData.gpsGPSQualityLeaf);
    message->numberSatellites.publish(gpsData.gpsNumberSatellitesLeaf);
    message->horizontalDilutionOfPrecision.publish(gpsData.gpsHorizontalDilutionOfPrecisionLeaf);
    message->antennaAltitude.publish(gpsData.gpsAltitudeLeaf);
    message->geoidalSeparation.publish(gpsData.gpsGeoidalSeparationLeaf);
    message->gpsDataAge.publish(gpsData.gpsDataAgeLeaf);
    message->differentialReferenceStation.publish(gpsData.gpsDifferentialReferenceStationLeaf);
    gpsData.endUpdates();
}

void GPSBridge::bridgeNMEAGLLMessage(const NMEAGLLMessage *message) {
    gpsData.beginUpdates();
    message->latitude.publish(gpsData.gpsLatitudeLeaf);
    message->longitude.publish(gpsData.gpsLongitudeLeaf);
    message->time.publish(gpsData.gpsTimeLeaf);
    message->dataValid.publish(gpsData.gpsDataValidLeaf);
    message->faaModeIndicator.publish(gpsData.gpsFAAModeindicatorLeaf);
    gpsData.endUpdates();
}

// The Vesper GPS receivers, and possibly others, emit back to back GSA messages with two sets of
// satellite IDs. This should be somehow detected so that the two groups can be concatenated
// together.
void GPSBridge::bridgeNMEAGSAMessage(const NMEAGSAMessage *message) {
    gpsData.beginUpdates();
    if (message->automaticMode) {
        gpsData.gpsSatelliteSelectionModeLeaf = "Automatic";
    } else {
        gpsData.gpsSatelliteSelectionModeLeaf = "Manual";
    }
    message->gpsFixMode.publish(gpsData.gpsFixModeLeaf);

    etl::string<activeSatellitesLength> activeSatellitesStr;
    etl::string_stream activeSatellitesStrStream(activeSatellitesStr);
    bool firstInList = true;
    for (unsigned satelliteIndex = 0; satelliteIndex < 12; satelliteIndex++) {
        if (message->satelliteIDs[satelliteIndex].hasValue()) {
            if (!firstInList) {
                activeSatellitesStrStream << ",";
            } else {
                firstInList = false;
            }
            activeSatellitesStrStream << message->satelliteIDs[satelliteIndex].getValue();
        }
    }

    gpsData.gpsActiveSatellitesLeaf = activeSatellitesStr;

    message->pdop.publish(gpsData.gpsPDOPLeaf);
    message->hdop.publish(gpsData.gpsHDOPLeaf);
    message->vdop.publish(gpsData.gpsVDOPLeaf);
    gpsData.endUpdates();
}

void GPSBridge::bridgeNMEAGSTMessage(const NMEAGSTMessage *message) {
    gpsData.beginUpdates();
    message->standardDeviationOfRangeInputsRMS.publish(gpsData.gpsStandardDeviationOfRangeInputsRMSLeaf);
    message->standardDeviationOfSemiMajorAxis.publish(gpsData.gpsStandardDeviationOfSemiMajorAxisLeaf);
    message->standardDeviationOfSemiMinorAxis.publish(gpsData.gpsStandardDeviationOfSemiMinorAxisLeaf);
    message->orientationOfSemiMajorAxis.publish(gpsData.gpsOrientationOfSemiMajorAxisLeaf);
    message->standardDeviationOfLatitudeError.publish(gpsData.gpsStandardDeviationOfLatitudeErrorLeaf);
    message->standardDeviationOfLongitudeError.publish(gpsData.gpsStandardDeviationOfLongitudeErrorLeaf);
    message->standardDeviationOfAltitudeError.publish(gpsData.gpsStandardDeviationOfAltitudeErrorLeaf);
    gpsData.endUpdates();
}

void GPSBridge::bridgeNMEARMCMessage(const NMEARMCMessage *message) {
    gpsData.beginUpdates();
    message->time.publish(gpsData.gpsTimeLeaf);
    message->dataValid.publish(gpsData.gpsDataValidLeaf);
    message->latitude.publish(gpsData.gpsLatitudeLeaf);
    message->longitude.publish(gpsData.gpsLongitudeLeaf);
    message->speedOverGround.publish(gpsData.gpsSpeedOverGroundLeaf);
    message->trackMadeGood.publish(gpsData.gpsTrackMadeGoodTrueLeaf);
    message->date.publish(gpsData.gpsDateLeaf);
    message->magneticVariation.publish(gpsData.gpsMagneticVariationLeaf);
    message->faaModeIndicator.publish(gpsData.gpsFAAModeindicatorLeaf);
    gpsData.endUpdates();
}

void GPSBridge::bridgeNMEAVTGMessage(const NMEAVTGMessage *message) {
    gpsData.beginUpdates();
    message->trackMadeGoodTrue.publish(gpsData.gpsTrackMadeGoodTrueLeaf);
    message->trackMadeGoodMagnetic.publish(gpsData.gpsTrackMadeGoodMagneticLeaf);
    message->speedOverGround.publish(gpsData.gpsSpeedOverGroundLeaf);
    message->speedOverGroundKmPerH.publish(gpsData.gpsSpeedOverGroundKmPerHLeaf);
    message->faaModeIndicator.publish(gpsData.gpsFAAModeindicatorLeaf);
    gpsData.endUpdates();
}
