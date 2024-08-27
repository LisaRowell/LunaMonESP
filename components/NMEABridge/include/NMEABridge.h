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

#ifndef NMEA_BRIDGE_H
#define NMEA_BRIDGE_H

#include "NMEALineHandler.h"
#include "NMEAMsgType.h"

#include "StatsHolder.h"
#include "StatCounter.h"

#include "DataModelNode.h"
#include "DataModelUInt32Leaf.h"

#include "etl/set.h"

#include <stddef.h>
#include <stdint.h>

class NMEALine;
class NMEAInterface;
class Interface;
class StatsManager;
class DataModel;

/*
 * This NMEA bridge is configured to get messages from a single interface, queuing them on the
 * output interface if there is room. If the TX queue doesn't have room for the message we drop
 * it rather than blocking the potentially higher rate input interface. 
 */
class NMEABridge : NMEALineHandler, StatsHolder {
    private:
        static constexpr size_t MAX_BRIDGE_MSG_TYPES = 10;

        etl::set<NMEAMsgType, MAX_BRIDGE_MSG_TYPES> bridgedMsgTypes;
        const char *name;
        Interface &dstInterface;
        StatCounter bridgedMessages;
        uint32_t droppedMessages;
        DataModelNode bridgeNode;
        DataModelUInt32Leaf bridgedMessagesLeaf;
        DataModelUInt32Leaf bridgedMessageRateLeaf;
        DataModelUInt32Leaf droppedMessageLeaf;

        void buildBridgedMessageSet(const char *msgTypeList);
        virtual void handleLine(const NMEALine &inputLine) override;
        void parseMsgType(NMEAMsgType &msgType, const NMEALine &inputLine);
        virtual void exportStats(uint32_t msElapsed) override;

    public:
        NMEABridge(const char *name, const char *msgTypeList, NMEAInterface &srcInterface,
                   Interface &dstInterface, StatsManager &statsManager, DataModel &dataModel);
};

#endif // NMEA_BRIDGE_H
