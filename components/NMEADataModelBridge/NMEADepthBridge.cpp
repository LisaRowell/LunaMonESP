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

#include "NMEADepthBridge.h"

#include "NMEADBKMessage.h"
#include "NMEADBSMessage.h"
#include "NMEADBTMessage.h"

#include "DataModel.h"
#include "DataModelNode.h"
#include "DataModelTenthsUInt16Leaf.h"

#include "StatCounter.h"

NMEADepthBridge::NMEADepthBridge(DataModel &dataModel, StatCounter &messagesBridgedCounter)
    : messagesBridgedCounter(messagesBridgedCounter),
      depthNode("depth", &dataModel.rootNode()),
      depthBelowSurfaceNode("belowSurface", &depthNode),
      depthBelowSurfaceFeetLeaf("feet", &depthBelowSurfaceNode),
      depthBelowSurfaceMetersLeaf("meters", &depthBelowSurfaceNode),
      depthBelowSurfaceFathomsLeaf("fathoms", &depthBelowSurfaceNode),
      depthBelowTransducerNode("belowTransducer", &depthNode),
      depthBelowTransducerFeetLeaf("feet", &depthBelowTransducerNode),
      depthBelowTransducerMetersLeaf("meters", &depthBelowTransducerNode),
      depthBelowTransducerFathomsLeaf("fathoms", &depthBelowTransducerNode),
      depthBelowKeelNode("belowKeel", &depthNode),
      depthBelowKeelFeetLeaf("feet", &depthBelowKeelNode),
      depthBelowKeelMetersLeaf("meters", &depthBelowKeelNode),
      depthBelowKeelFathomsLeaf("fathoms", &depthBelowKeelNode)
{
}

void NMEADepthBridge::bridgeNMEADBKMessage(const NMEADBKMessage *message) {
    message->depthFeet.publish(depthBelowKeelFeetLeaf);
    message->depthMeters.publish(depthBelowKeelMetersLeaf);
    message->depthFathoms.publish(depthBelowKeelFathomsLeaf);

    messagesBridgedCounter++;
}

void NMEADepthBridge::bridgeNMEADBSMessage(const NMEADBSMessage *message) {
    message->depthFeet.publish(depthBelowSurfaceFeetLeaf);
    message->depthMeters.publish(depthBelowSurfaceMetersLeaf);
    message->depthFathoms.publish(depthBelowSurfaceFathomsLeaf);

    messagesBridgedCounter++;
}

void NMEADepthBridge::bridgeNMEADBTMessage(const NMEADBTMessage *message) {
    message->depthFeet.publish(depthBelowTransducerFeetLeaf);
    message->depthMeters.publish(depthBelowTransducerMetersLeaf);
    message->depthFathoms.publish(depthBelowTransducerFathomsLeaf);

    messagesBridgedCounter++;
}
