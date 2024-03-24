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

#ifndef NMEA_DATA_MODEL_BRIDGE_H
#define NMEA_DATA_MODEL_BRIDGE_H

#include "NMEAMessageHandler.h"

#include "NMEADepthBridge.h"
#include "NMEAGPSBridge.h"

#include "DataModelNode.h"
#include "DataModelUInt32Leaf.h"

#include "StatCounter.h"

#include "StatsHolder.h"

class DataModel;
class StatsManager;
class NMEAMessage;
class NMEATXTMessage;

class NMEADataModelBridge : public NMEAMessageHandler, StatsHolder {
    private:
        NMEADepthBridge depthBridge;
        NMEAGPSBridge gpsBridge;

        StatCounter messagesBridgedCounter;

        DataModelNode nmeaDataModelBridgeNode;
        DataModelUInt32Leaf messagesBridgedLeaf;
        DataModelUInt32Leaf messagesBridgedRateLeaf;

        void logTXTMessage(NMEATXTMessage *message);
        virtual void exportStats(uint32_t msElapsed) override;

    public:
        NMEADataModelBridge(DataModel &dataModel, StatsManager &statsManager);
        virtual void processMessage(NMEAMessage *message) override;
};

#endif // NMEA_DATA_MODEL_BRIDGE_H
