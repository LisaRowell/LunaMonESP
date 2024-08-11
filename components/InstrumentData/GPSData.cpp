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

#include "GPSData.h"
#include "InstrumentGroup.h"

#include "DataModel.h"
#include "DataModelNode.h"
#include "DataModelBoolLeaf.h"
#include "DataModelUInt16Leaf.h"
#include "DataModelTenthsInt16Leaf.h"
#include "DataModelTenthsUInt16Leaf.h"
#include "DataModelHundredthsUInt8Leaf.h"
#include "DataModelHundredthsUInt16Leaf.h"
#include "DataModelStringLeaf.h"

#include "etl/string.h"

GPSData::GPSData(DataModel &dataModel, DataModelNode &instrumentDataNode,
                 StatsManager &statsManager)
    : InstrumentGroup("GPS", "gps", instrumentDataNode, statsManager),
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
      gpsStandardDeviationOfAltitudeErrorLeaf("standardDeviationOfAltitudeError", &gpsNode) {
}
