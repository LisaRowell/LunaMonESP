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

#ifndef SEA_TALK_NMEA_BRIDGE_H
#define SEA_TALK_NMEA_BRIDGE_H

#include "StatsHolder.h"
#include "StatCounter.h"

#include "DataModelNode.h"
#include "DataModelUInt32Leaf.h"
#include "DataModelStringLeaf.h"

#include "etl/string.h"

#include <stdint.h>

class NMEALineHandler;
class StatsManager;
class DataModel;

class SeaTalkNMEABridge : StatsHolder {
    private:
        static constexpr size_t maxLabelLength = 20;

        const char *talkerCode;
        NMEALineHandler &destination;
        StatCounter bridgedMessages;
        DataModelNode bridgeNode;
        etl::string<maxLabelLength> labelBuffer;
        DataModelStringLeaf labelLeaf;
        DataModelUInt32Leaf bridgedMessagesLeaf;
        DataModelUInt32Leaf bridgedMessageRateLeaf;

        void bridgeMessage(const char *typeCode, const etl::istring &message);
        virtual void exportStats(uint32_t msElapsed) override;

    public:
        SeaTalkNMEABridge(const char *name, const char *label, const char *talkerCode,
                          NMEALineHandler &destination, StatsManager &statsManager,
                          DataModel &dataModel);
        void bridgeHDMMessage(uint16_t heading);
};

#endif // SEA_TALK_NMEA_BRIDGE_H
