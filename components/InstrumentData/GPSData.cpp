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
      timeLeaf("time", &gpsNode, timeBuffer),
      dateLeaf("date", &gpsNode, dateBuffer),
      dataValidLeaf("dataValid", &gpsNode),
      latitudeLeaf("latitude", &gpsNode, latitudeBuffer),
      longitudeLeaf("longitude", &gpsNode, longitudeBuffer),
      altitudeLeaf("altitude", &gpsNode),
      speedOverGroundLeaf("speedOverGround", &gpsNode),
      speedOverGroundKmPerHLeaf("speedOverGroundKmPerH", &gpsNode),
      trackMadeGoodTrueLeaf("trackMadeGoodTrue", &gpsNode),
      trackMadeGoodMagneticLeaf("trackMadeGoodMagnetic", &gpsNode),
      magneticVariationLeaf("magneticVariation", &gpsNode),
      faaModeindicatorLeaf("faaModeIndicator", &gpsNode, faaModeIndicatorBuffer),
      gpsQualityLeaf("gpsQuality", &gpsNode, gpsQualityBuffer),
      numberSatellitesLeaf("numberSatellites", &gpsNode),
      horizontalDilutionOfPrecisionLeaf("horizontalDilutionOfPrecision", &gpsNode),
      geoidalSeparationLeaf("geoidalSeparation", &gpsNode),
      dataAgeLeaf("dataAge", &gpsNode),
      differentialReferenceStationLeaf("differentialReferenceStation", &gpsNode),
      satelliteSelectionModeLeaf("satelliteSelectionMode", &gpsNode, satelliteSelectionModeBuffer),
      fixModeLeaf("fixMode", &gpsNode, fixModeBuffer),
      activeSatellitesLeaf("activeSatellites", &gpsNode, activeSatellitesBuffer),
      pdopLeaf("pdop", &gpsNode),
      hdopLeaf("hdop", &gpsNode),
      vdopLeaf("vdop", &gpsNode),
      standardDeviationOfRangeInputsRMSLeaf("standardDeviationOfRangeInputsRMS", &gpsNode),
      standardDeviationOfSemiMajorAxisLeaf("standardDeviationOfSemiMajorAxis", &gpsNode),
      standardDeviationOfSemiMinorAxisLeaf("standardDeviationOfSemiMinorAxis", &gpsNode),
      orientationOfSemiMajorAxisLeaf("orientationOfSemiMajorAxis", &gpsNode),
      standardDeviationOfLatitudeErrorLeaf("standardDeviationOfLatitudeError", &gpsNode),
      standardDeviationOfLongitudeErrorLeaf("standardDeviationOfLongitudeError", &gpsNode),
      standardDeviationOfAltitudeErrorLeaf("standardDeviationOfAltitudeError", &gpsNode) {
}
