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
        etl::string<timeLength> timeBuffer;
        etl::string<dateLength> dateBuffer;
        etl::string<coordinateLength> latitudeBuffer;
        etl::string<coordinateLength> longitudeBuffer;
        etl::string<15> faaModeIndicatorBuffer;
        etl::string<20> gpsQualityBuffer;
        etl::string<9> satelliteSelectionModeBuffer;
        etl::string<4> fixModeBuffer;
        etl::string<activeSatellitesLength> activeSatellitesBuffer;

    public:
        DataModelStringLeaf timeLeaf;
        DataModelStringLeaf dateLeaf;
        DataModelBoolLeaf dataValidLeaf;
        DataModelStringLeaf latitudeLeaf;
        DataModelStringLeaf longitudeLeaf;
        DataModelTenthsInt16Leaf altitudeLeaf;
        DataModelTenthsUInt16Leaf speedOverGroundLeaf;
        DataModelTenthsUInt16Leaf speedOverGroundKmPerHLeaf;
        DataModelTenthsUInt16Leaf trackMadeGoodTrueLeaf;
        DataModelTenthsUInt16Leaf trackMadeGoodMagneticLeaf;
        DataModelTenthsInt16Leaf magneticVariationLeaf;
        DataModelStringLeaf faaModeindicatorLeaf;
        DataModelStringLeaf gpsQualityLeaf;
        DataModelUInt16Leaf numberSatellitesLeaf;
        DataModelHundredthsUInt16Leaf horizontalDilutionOfPrecisionLeaf;
        DataModelTenthsInt16Leaf geoidalSeparationLeaf;
        DataModelTenthsUInt16Leaf dataAgeLeaf;
        DataModelUInt16Leaf differentialReferenceStationLeaf;
        DataModelStringLeaf satelliteSelectionModeLeaf;
        DataModelStringLeaf fixModeLeaf;
        DataModelStringLeaf activeSatellitesLeaf;
        DataModelHundredthsUInt8Leaf pdopLeaf;
        DataModelHundredthsUInt8Leaf hdopLeaf;
        DataModelHundredthsUInt8Leaf vdopLeaf;
        DataModelTenthsUInt16Leaf standardDeviationOfRangeInputsRMSLeaf;
        DataModelTenthsUInt16Leaf standardDeviationOfSemiMajorAxisLeaf;
        DataModelTenthsUInt16Leaf standardDeviationOfSemiMinorAxisLeaf;
        DataModelTenthsUInt16Leaf orientationOfSemiMajorAxisLeaf;
        DataModelTenthsUInt16Leaf standardDeviationOfLatitudeErrorLeaf;
        DataModelTenthsUInt16Leaf standardDeviationOfLongitudeErrorLeaf;
        DataModelTenthsUInt16Leaf standardDeviationOfAltitudeErrorLeaf;

        GPSData(DataModel &dataModel, DataModelNode &instrumentDataNode,
                StatsManager &statsManager);
};

#endif // GPS_DATA_H
