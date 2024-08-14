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
    message->time.publish(gpsData.timeLeaf);
    message->latitude.publish(gpsData.latitudeLeaf);
    message->longitude.publish(gpsData.longitudeLeaf);
    message->gpsQuality.publish(gpsData.gpsQualityLeaf);
    message->numberSatellites.publish(gpsData.numberSatellitesLeaf);
    message->horizontalDilutionOfPrecision.publish(gpsData.horizontalDilutionOfPrecisionLeaf);
    message->antennaAltitude.publish(gpsData.altitudeLeaf);
    message->geoidalSeparation.publish(gpsData.geoidalSeparationLeaf);
    message->gpsDataAge.publish(gpsData.dataAgeLeaf);
    message->differentialReferenceStation.publish(gpsData.differentialReferenceStationLeaf);
    gpsData.endUpdates();
}

void GPSBridge::bridgeNMEAGLLMessage(const NMEAGLLMessage *message) {
    gpsData.beginUpdates();
    message->latitude.publish(gpsData.latitudeLeaf);
    message->longitude.publish(gpsData.longitudeLeaf);
    message->time.publish(gpsData.timeLeaf);
    message->dataValid.publish(gpsData.dataValidLeaf);
    message->faaModeIndicator.publish(gpsData.faaModeindicatorLeaf);
    gpsData.endUpdates();
}

// The Vesper GPS receivers, and possibly others, emit back to back GSA messages with two sets of
// satellite IDs. This should be somehow detected so that the two groups can be concatenated
// together.
void GPSBridge::bridgeNMEAGSAMessage(const NMEAGSAMessage *message) {
    gpsData.beginUpdates();
    if (message->automaticMode) {
        gpsData.satelliteSelectionModeLeaf = "Automatic";
    } else {
        gpsData.satelliteSelectionModeLeaf = "Manual";
    }
    message->gpsFixMode.publish(gpsData.fixModeLeaf);

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

    gpsData.activeSatellitesLeaf = activeSatellitesStr;

    message->pdop.publish(gpsData.pdopLeaf);
    message->hdop.publish(gpsData.hdopLeaf);
    message->vdop.publish(gpsData.vdopLeaf);
    gpsData.endUpdates();
}

void GPSBridge::bridgeNMEAGSTMessage(const NMEAGSTMessage *message) {
    gpsData.beginUpdates();
    message->standardDeviationOfRangeInputsRMS.publish(gpsData.standardDeviationOfRangeInputsRMSLeaf);
    message->standardDeviationOfSemiMajorAxis.publish(gpsData.standardDeviationOfSemiMajorAxisLeaf);
    message->standardDeviationOfSemiMinorAxis.publish(gpsData.standardDeviationOfSemiMinorAxisLeaf);
    message->orientationOfSemiMajorAxis.publish(gpsData.orientationOfSemiMajorAxisLeaf);
    message->standardDeviationOfLatitudeError.publish(gpsData.standardDeviationOfLatitudeErrorLeaf);
    message->standardDeviationOfLongitudeError.publish(gpsData.standardDeviationOfLongitudeErrorLeaf);
    message->standardDeviationOfAltitudeError.publish(gpsData.standardDeviationOfAltitudeErrorLeaf);
    gpsData.endUpdates();
}

void GPSBridge::bridgeNMEARMCMessage(const NMEARMCMessage *message) {
    gpsData.beginUpdates();
    message->time.publish(gpsData.timeLeaf);
    message->dataValid.publish(gpsData.dataValidLeaf);
    message->latitude.publish(gpsData.latitudeLeaf);
    message->longitude.publish(gpsData.longitudeLeaf);
    message->speedOverGround.publish(gpsData.speedOverGroundLeaf);
    message->trackMadeGood.publish(gpsData.trackMadeGoodTrueLeaf);
    message->date.publish(gpsData.dateLeaf);
    message->magneticVariation.publish(gpsData.magneticVariationLeaf);
    message->faaModeIndicator.publish(gpsData.faaModeindicatorLeaf);
    gpsData.endUpdates();
}

void GPSBridge::bridgeNMEAVTGMessage(const NMEAVTGMessage *message) {
    gpsData.beginUpdates();
    message->trackMadeGoodTrue.publish(gpsData.trackMadeGoodTrueLeaf);
    message->trackMadeGoodMagnetic.publish(gpsData.trackMadeGoodMagneticLeaf);
    message->speedOverGround.publish(gpsData.speedOverGroundLeaf);
    message->speedOverGroundKmPerH.publish(gpsData.speedOverGroundKmPerHLeaf);
    message->faaModeIndicator.publish(gpsData.faaModeindicatorLeaf);
    gpsData.endUpdates();
}
