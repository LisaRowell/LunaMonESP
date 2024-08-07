/*
 * This file is part of LunaMon (https://github.com/LisaRowell/LunaMonESP)
 * Copyright (C) 2023-2024 Lisa Rowell
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

#ifndef NMEA_DEPTH_BRIDGE_H
#define NMEA_DEPTH_BRIDGE_H

#include "DataModelNode.h"
#include "DataModelTenthsInt16Leaf.h"
#include "DataModelTenthsUInt16Leaf.h"

class DataModel;
class NMEADBKMessage;
class NMEADBSMessage;
class NMEADBTMessage;
class NMEADPTMessage;
class StatCounter;

class NMEADepthBridge {
    private:
        StatCounter &messagesBridgedCounter;

        DataModelNode depthNode;
        DataModelNode depthBelowSurfaceNode;
        DataModelTenthsUInt16Leaf depthBelowSurfaceFeetLeaf;
        DataModelTenthsUInt16Leaf depthBelowSurfaceMetersLeaf;
        DataModelTenthsUInt16Leaf depthBelowSurfaceFathomsLeaf;
        DataModelNode depthBelowTransducerNode;
        DataModelTenthsUInt16Leaf depthBelowTransducerFeetLeaf;
        DataModelTenthsUInt16Leaf depthBelowTransducerMetersLeaf;
        DataModelTenthsUInt16Leaf depthBelowTransducerFathomsLeaf;
        DataModelNode depthBelowKeelNode;
        DataModelTenthsInt16Leaf depthBelowKeelFeetLeaf;
        DataModelTenthsInt16Leaf depthBelowKeelMetersLeaf;
        DataModelTenthsInt16Leaf depthBelowKeelFathomsLeaf;

    public:
        NMEADepthBridge(DataModel &dataModel, StatCounter &messagesBridgedCounter);
        void bridgeNMEADBKMessage(const NMEADBKMessage *message);
        void bridgeNMEADBSMessage(const NMEADBSMessage *message);
        void bridgeNMEADBTMessage(const NMEADBTMessage *message);
        void bridgeNMEADPTMessage(const NMEADPTMessage *message);
};

#endif // NMEA_DEPTH_BRIDGE_H
