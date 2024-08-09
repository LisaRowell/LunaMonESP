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

#ifndef WATER_BRIDGE_H
#define WATER_BRIDGE_H

#include "DataModelNode.h"
#include "DataModelTenthsInt16Leaf.h"
#include "DataModelTenthsUInt16Leaf.h"

class DataModel;
class NMEAMTWMessage;
class NMEAVHWMessage;
class StatCounter;

class WaterBridge {
    private:
        StatCounter &messagesBridgedCounter;

        DataModelNode waterNode;
        DataModelNode waterHeadingNode;
        DataModelTenthsUInt16Leaf waterHeadingTrueLeaf;
        DataModelTenthsUInt16Leaf waterHeadingMagneticLeaf;
        DataModelNode waterSpeedNode;
        DataModelTenthsInt16Leaf waterSpeedKnotsLeaf;
        DataModelTenthsInt16Leaf waterSpeedKMPHLeaf;
        DataModelNode waterTemperatureNode;
        DataModelTenthsInt16Leaf waterTemperatureCelsiusLeaf;
        DataModelTenthsInt16Leaf waterTemperatureFahrenheitLeaf;

    public:
        WaterBridge(DataModel &dataModel, StatCounter &messagesBridgedCounter);
        void bridgeNMEAMTWMessage(const NMEAMTWMessage *message);
        void bridgeNMEAVHWMessage(const NMEAVHWMessage *message);
};

#endif // WATER_BRIDGE_H
