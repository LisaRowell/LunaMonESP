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

#ifndef WIND_DATA_H
#define WIND_DATA_H

#include "InstrumentGroup.h"

#include "DataModelNode.h"
#include "DataModelBoolLeaf.h"
#include "DataModelTenthsUInt16Leaf.h"

class DataModel;
class StatsManager;

class WindData : public InstrumentGroup {
    private:
        DataModelNode windNode;
        DataModelNode apparentWindNode;
        DataModelNode apparentWindSpeedNode;
        DataModelNode trueWindNode;
        DataModelNode trueWindSpeedNode;

    public:
        DataModelTenthsUInt16Leaf apparentWindAngleLeaf;
        DataModelTenthsUInt16Leaf apparentWindSpeedKnotsLeaf;
        DataModelTenthsUInt16Leaf apparentWindSpeedMPHLeaf;
        DataModelTenthsUInt16Leaf apparentWindSpeedKMPHLeaf;
        DataModelTenthsUInt16Leaf apparentWindSpeedMPSLeaf;
        DataModelTenthsUInt16Leaf trueWindAngleLeaf;
        DataModelTenthsUInt16Leaf trueWindSpeedKnotsLeaf;
        DataModelTenthsUInt16Leaf trueWindSpeedMPHLeaf;
        DataModelTenthsUInt16Leaf trueWindSpeedKMPHLeaf;
        DataModelBoolLeaf windValidLeaf;

        WindData(DataModel &dataModel, DataModelNode &instrumentDataNode,
                 StatsManager &statsManager);
};

#endif // WIND_DATA_H
