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

#include "NMEAGGAMessage.h"
#include "NMEAGLLMessage.h"
#include "NMEAGSAMessage.h"
#include "NMEAGSTMessage.h"
#include "NMEARMCMessage.h"
#include "NMEAVTGMessage.h"

#include "DataModel.h"
#include "DataModelNode.h"
#include "DataModelStringLeaf.h"
#include "DataModelTenthsInt16Leaf.h"

#include "StatCounter.h"

#include "etl/string.h"
#include "etl/string_stream.h"

GPSBridge::GPSBridge(DataModel &dataModel, StatCounter &messagesBridgedCounter)
    : messagesBridgedCounter(messagesBridgedCounter),
      gpsNode("gps", &dataModel.rootNode()),
      gpsTimeLeaf("time", &gpsNode, gpsTimeBuffer),
      gpsDateLeaf("date", &gpsNode, gpsDateBuffer),
      gpsDataValidLeaf("dataValid", &gpsNode),
      gpsLatitudeLeaf("latitude", &gpsNode, gpsLatitudeBuffer),
      gpsLongitudeLeaf("longitude", &gpsNode, gpsLongitudeBuffer),
      gpsAltitudeLeaf("altitude", &gpsNode),
      gpsSpeedOverGroundLeaf("speedOverGround", &gpsNode),
      gpsSpeedOverGroundKmPerHLeaf("speedOverGroundKmPerH", &gpsNode),
      gpsTrackMadeGoodTrueLeaf("trackMadeGoodTrue", &gpsNode),
      gpsTrackMadeGoodMagneticLeaf("trackMadeGoodMagnetic", &gpsNode),
      gpsMagneticVariationLeaf("magneticVariation", &gpsNode),
      gpsFAAModeindicatorLeaf("faaModeIndicator", &gpsNode, gpsFAAModeIndicatorBuffer),
      gpsGPSQualityLeaf("gpsQuality", &gpsNode, gpsGPSQualityBuffer),
      gpsNumberSatellitesLeaf("numberSatellites", &gpsNode),
      gpsHorizontalDilutionOfPrecisionLeaf("horizontalDilutionOfPrecision", &gpsNode),
      gpsGeoidalSeparationLeaf("geoidalSeparation", &gpsNode),
      gpsDataAgeLeaf("dataAge", &gpsNode),
      gpsDifferentialReferenceStationLeaf("differentialReferenceStation", &gpsNode),
      gpsSatelliteSelectionModeLeaf("satelliteSelectionMode", &gpsNode,
                                    gpsSatelliteSelectionModeBuffer),
      gpsFixModeLeaf("fixMode", &gpsNode, gpsFixModeBuffer),
      gpsActiveSatellitesLeaf("activeSatellites", &gpsNode, gpsActiveSatellitesBuffer),
      gpsPDOPLeaf("pdop", &gpsNode),
      gpsHDOPLeaf("hdop", &gpsNode),
      gpsVDOPLeaf("vdop", &gpsNode),
      gpsStandardDeviationOfRangeInputsRMSLeaf("standardDeviationOfRangeInputsRMS", &gpsNode),
      gpsStandardDeviationOfSemiMajorAxisLeaf("standardDeviationOfSemiMajorAxis", &gpsNode),
      gpsStandardDeviationOfSemiMinorAxisLeaf("standardDeviationOfSemiMinorAxis", &gpsNode),
      gpsOrientationOfSemiMajorAxisLeaf("orientationOfSemiMajorAxis", &gpsNode),
      gpsStandardDeviationOfLatitudeErrorLeaf("standardDeviationOfLatitudeError", &gpsNode),
      gpsStandardDeviationOfLongitudeErrorLeaf("standardDeviationOfLongitudeError", &gpsNode),
      gpsStandardDeviationOfAltitudeErrorLeaf("standardDeviationOfAltitudeError", &gpsNode)
{
}

void GPSBridge::bridgeNMEAGGAMessage(const NMEAGGAMessage *message) {
    message->time.publish(gpsTimeLeaf);
    message->latitude.publish(gpsLatitudeLeaf);
    message->longitude.publish(gpsLongitudeLeaf);
    message->gpsQuality.publish(gpsGPSQualityLeaf);
    message->numberSatellites.publish(gpsNumberSatellitesLeaf);
    message->horizontalDilutionOfPrecision.publish(gpsHorizontalDilutionOfPrecisionLeaf);
    message->antennaAltitude.publish(gpsAltitudeLeaf);
    message->geoidalSeparation.publish(gpsGeoidalSeparationLeaf);
    message->gpsDataAge.publish(gpsDataAgeLeaf);
    message->differentialReferenceStation.publish(gpsDifferentialReferenceStationLeaf);

    messagesBridgedCounter++;
}

void GPSBridge::bridgeNMEAGLLMessage(const NMEAGLLMessage *message) {
    message->latitude.publish(gpsLatitudeLeaf);
    message->longitude.publish(gpsLongitudeLeaf);
    message->time.publish(gpsTimeLeaf);
    message->dataValid.publish(gpsDataValidLeaf);
    message->faaModeIndicator.publish(gpsFAAModeindicatorLeaf);

    messagesBridgedCounter++;
}

// The Vesper GPS receivers, and possibly others, emit back to back GSA messages with two sets of
// satellite IDs. This should be somehow detected so that the two groups can be concatenated
// together.
void GPSBridge::bridgeNMEAGSAMessage(const NMEAGSAMessage *message) {
    if (message->automaticMode) {
        gpsSatelliteSelectionModeLeaf = "Automatic";
    } else {
        gpsSatelliteSelectionModeLeaf = "Manual";
    }
    message->gpsFixMode.publish(gpsFixModeLeaf);

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

    gpsActiveSatellitesLeaf = activeSatellitesStr;

    message->pdop.publish(gpsPDOPLeaf);
    message->hdop.publish(gpsHDOPLeaf);
    message->vdop.publish(gpsVDOPLeaf);

    messagesBridgedCounter++;
}

void GPSBridge::bridgeNMEAGSTMessage(const NMEAGSTMessage *message) {
    message->standardDeviationOfRangeInputsRMS.publish(gpsStandardDeviationOfRangeInputsRMSLeaf);
    message->standardDeviationOfSemiMajorAxis.publish(gpsStandardDeviationOfSemiMajorAxisLeaf);
    message->standardDeviationOfSemiMinorAxis.publish(gpsStandardDeviationOfSemiMinorAxisLeaf);
    message->orientationOfSemiMajorAxis.publish(gpsOrientationOfSemiMajorAxisLeaf);
    message->standardDeviationOfLatitudeError.publish(gpsStandardDeviationOfLatitudeErrorLeaf);
    message->standardDeviationOfLongitudeError.publish(gpsStandardDeviationOfLongitudeErrorLeaf);
    message->standardDeviationOfAltitudeError.publish(gpsStandardDeviationOfAltitudeErrorLeaf);

    messagesBridgedCounter++;
}

void GPSBridge::bridgeNMEARMCMessage(const NMEARMCMessage *message) {
    message->time.publish(gpsTimeLeaf);
    message->dataValid.publish(gpsDataValidLeaf);
    message->latitude.publish(gpsLatitudeLeaf);
    message->longitude.publish(gpsLongitudeLeaf);
    message->speedOverGround.publish(gpsSpeedOverGroundLeaf);
    message->trackMadeGood.publish(gpsTrackMadeGoodTrueLeaf);
    message->date.publish(gpsDateLeaf);
    message->magneticVariation.publish(gpsMagneticVariationLeaf);
    message->faaModeIndicator.publish(gpsFAAModeindicatorLeaf);

    messagesBridgedCounter++;
}

void GPSBridge::bridgeNMEAVTGMessage(const NMEAVTGMessage *message) {
    message->trackMadeGoodTrue.publish(gpsTrackMadeGoodTrueLeaf);
    message->trackMadeGoodMagnetic.publish(gpsTrackMadeGoodMagneticLeaf);
    message->speedOverGround.publish(gpsSpeedOverGroundLeaf);
    message->speedOverGroundKmPerH.publish(gpsSpeedOverGroundKmPerHLeaf);
    message->faaModeIndicator.publish(gpsFAAModeindicatorLeaf);

    messagesBridgedCounter++;
}
