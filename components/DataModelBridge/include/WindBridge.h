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

#ifndef WIND_BRIDGE_H
#define WIND_BRIDGE_H

#include "DataModelNode.h"
#include "DataModelTenthsUInt16Leaf.h"
#include "DataModelBoolLeaf.h"

class DataModel;
class NMEAMWVMessage;
class StatCounter;

class WindBridge {
    private:
        StatCounter &messagesBridgedCounter;

        DataModelNode windNode;
        DataModelNode apparentWindNode;
        DataModelTenthsUInt16Leaf apparentWindAngleLeaf;
        DataModelNode apparentWindSpeedNode;
        DataModelTenthsUInt16Leaf apparentWindSpeedKnotsLeaf;
        DataModelTenthsUInt16Leaf apparentWindSpeedMPHLeaf;
        DataModelTenthsUInt16Leaf apparentWindSpeedKMPHLeaf;
        DataModelNode trueWindNode;
        DataModelTenthsUInt16Leaf trueWindAngleLeaf;
        DataModelNode trueWindSpeedNode;
        DataModelTenthsUInt16Leaf trueWindSpeedKnotsLeaf;
        DataModelTenthsUInt16Leaf trueWindSpeedMPHLeaf;
        DataModelTenthsUInt16Leaf trueWindSpeedKMPHLeaf;
        DataModelBoolLeaf windValidLeaf;

    public:
        WindBridge(DataModel &dataModel, StatCounter &messagesBridgedCounter);
        void bridgeNMEAMWVMessage(const NMEAMWVMessage *message);
};

#endif // WIND_BRIDGE_H
