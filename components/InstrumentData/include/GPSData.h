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

#ifndef GPS_DATA_H
#define GPS_DATA_H

#include "InstrumentGroup.h"

#include "DataModelNode.h"
#include "DataModelBoolLeaf.h"
#include "DataModelUInt16Leaf.h"
#include "DataModelTenthsInt16Leaf.h"
#include "DataModelTenthsUInt16Leaf.h"
#include "DataModelHundredthsUInt8Leaf.h"
#include "DataModelHundredthsUInt16Leaf.h"
#include "DataModelStringLeaf.h"

#include "etl/string.h"

class DataModel;
class StatsManager;

class GPSData : public InstrumentGroup {
    private:
        static constexpr size_t timeLength = 15;
        static constexpr size_t dateLength = 10;
        static constexpr size_t coordinateLength = 20;
        static constexpr size_t activeSatellitesLength = 72;

        DataModelNode gpsNode;
        etl::string<timeLength> gpsTimeBuffer;
        etl::string<dateLength> gpsDateBuffer;
        etl::string<coordinateLength> gpsLatitudeBuffer;
        etl::string<coordinateLength> gpsLongitudeBuffer;
        etl::string<15> gpsFAAModeIndicatorBuffer;
        etl::string<20> gpsGPSQualityBuffer;
        etl::string<9> gpsSatelliteSelectionModeBuffer;
        etl::string<4> gpsFixModeBuffer;
        etl::string<activeSatellitesLength> gpsActiveSatellitesBuffer;

    public:
        DataModelStringLeaf gpsTimeLeaf;
        DataModelStringLeaf gpsDateLeaf;
        DataModelBoolLeaf gpsDataValidLeaf;
        DataModelStringLeaf gpsLatitudeLeaf;
        DataModelStringLeaf gpsLongitudeLeaf;
        DataModelTenthsInt16Leaf gpsAltitudeLeaf;
        DataModelTenthsUInt16Leaf gpsSpeedOverGroundLeaf;
        DataModelTenthsUInt16Leaf gpsSpeedOverGroundKmPerHLeaf;
        DataModelTenthsUInt16Leaf gpsTrackMadeGoodTrueLeaf;
        DataModelTenthsUInt16Leaf gpsTrackMadeGoodMagneticLeaf;
        DataModelTenthsInt16Leaf gpsMagneticVariationLeaf;
        DataModelStringLeaf gpsFAAModeindicatorLeaf;
        DataModelStringLeaf gpsGPSQualityLeaf;
        DataModelUInt16Leaf gpsNumberSatellitesLeaf;
        DataModelHundredthsUInt16Leaf gpsHorizontalDilutionOfPrecisionLeaf;
        DataModelTenthsInt16Leaf gpsGeoidalSeparationLeaf;
        DataModelTenthsUInt16Leaf gpsDataAgeLeaf;
        DataModelUInt16Leaf gpsDifferentialReferenceStationLeaf;
        DataModelStringLeaf gpsSatelliteSelectionModeLeaf;
        DataModelStringLeaf gpsFixModeLeaf;
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

        GPSData(DataModel &dataModel, DataModelNode &instrumentDataNode,
                StatsManager &statsManager);
};

#endif // GPS_DATA_H
