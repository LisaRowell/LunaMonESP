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

#ifndef NMEA_GPS_BRIDGE_H
#define NMEA_GPS_BRIDGE_H

#include "DataModelNode.h"
#include "DataModelStringLeaf.h"
#include "DataModelBoolLeaf.h"
#include "DataModelUInt16Leaf.h"
#include "DataModelTenthsInt16Leaf.h"
#include "DataModelTenthsUInt16Leaf.h"
#include "DataModelHundredthsUInt8Leaf.h"
#include "DataModelHundredthsUInt16Leaf.h"

#include "etl/string.h"

#include <stddef.h>

class DataModel;
class NMEAGGAMessage;
class NMEAGLLMessage;
class NMEAGSAMessage;
class NMEAGSTMessage;
class NMEARMCMessage;
class NMEAVTGMessage;
class StatCounter;

class NMEAGPSBridge {
    private:
        static constexpr size_t timeLength = 15;
        static constexpr size_t dateLength = 10;
        static constexpr size_t coordinateLength = 20;
        static constexpr size_t activeSatellitesLength = 72;

        StatCounter &messagesBridgedCounter;

        DataModelNode gpsNode;
        etl::string<timeLength> gpsTimeBuffer;
        DataModelStringLeaf gpsTimeLeaf;
        etl::string<dateLength> gpsDateBuffer;
        DataModelStringLeaf gpsDateLeaf;
        DataModelBoolLeaf gpsDataValidLeaf;
        etl::string<coordinateLength> gpsLatitudeBuffer;
        DataModelStringLeaf gpsLatitudeLeaf;
        etl::string<coordinateLength> gpsLongitudeBuffer;
        DataModelStringLeaf gpsLongitudeLeaf;
        DataModelTenthsInt16Leaf gpsAltitudeLeaf;
        DataModelTenthsUInt16Leaf gpsSpeedOverGroundLeaf;
        DataModelTenthsUInt16Leaf gpsSpeedOverGroundKmPerHLeaf;
        DataModelTenthsUInt16Leaf gpsTrackMadeGoodTrueLeaf;
        DataModelTenthsUInt16Leaf gpsTrackMadeGoodMagneticLeaf;
        DataModelTenthsInt16Leaf gpsMagneticVariationLeaf;
        etl::string<15> gpsFAAModeIndicatorBuffer;
        DataModelStringLeaf gpsFAAModeindicatorLeaf;
        etl::string<20> gpsGPSQualityBuffer;
        DataModelStringLeaf gpsGPSQualityLeaf;
        DataModelUInt16Leaf gpsNumberSatellitesLeaf;
        DataModelHundredthsUInt16Leaf gpsHorizontalDilutionOfPrecisionLeaf;
        DataModelTenthsInt16Leaf gpsGeoidalSeparationLeaf;
        DataModelTenthsUInt16Leaf gpsDataAgeLeaf;
        DataModelUInt16Leaf gpsDifferentialReferenceStationLeaf;
        etl::string<9> gpsSatelliteSelectionModeBuffer;
        DataModelStringLeaf gpsSatelliteSelectionModeLeaf;
        etl::string<4> gpsFixModeBuffer;
        DataModelStringLeaf gpsFixModeLeaf;
        etl::string<activeSatellitesLength> gpsActiveSatellitesBuffer;
        DataModelStringLeaf gpsActiveSatellitesLeaf;
        DataModelHundredthsUInt8Leaf gpsPDOPLeaf;
        DataModelHundredthsUInt8Leaf gpsHDOPLeaf;
        DataModelHundredthsUInt8Leaf gpsVDOPLeaf;
        DataModelTenthsUInt16Leaf gpsStandardDeviationOfRangeInputsRMSLeaf;
        DataModelTenthsUInt16Leaf gpsStandardDeviationOfSemiMajorAxisLeaf;
        DataModelTenthsUInt16Leaf gpsStandardDeviationOfSemiMinorAxisLeaf;
        DataModelTenthsUInt16Leaf gpsOrientationOfSemiMajorAxisLeaf;
        DataModelTenthsUInt16Leaf gpsStandardDeviationOfLatitudeErrorLeaf;
        DataModelTenthsUInt16Leaf gpsStandardDeviationOfLongitudeErrorLeaf;
        DataModelTenthsUInt16Leaf gpsStandardDeviationOfAltitudeErrorLeaf;

    public:
        NMEAGPSBridge(DataModel &dataModel, StatCounter &messagesBridgedCounter);
        void bridgeNMEAGGAMessage(NMEAGGAMessage *message);
        void bridgeNMEAGLLMessage(NMEAGLLMessage *message);
        void bridgeNMEAGSAMessage(NMEAGSAMessage *message);
        void bridgeNMEAGSTMessage(NMEAGSTMessage *message);
        void bridgeNMEARMCMessage(NMEARMCMessage *message);
        void bridgeNMEAVTGMessage(NMEAVTGMessage *message);
};

#endif // NMEA_GPS_BRIDGE_H
