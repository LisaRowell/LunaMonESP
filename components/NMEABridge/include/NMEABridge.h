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

#include "TaskObject.h"
#include "NMEALineHandler.h"
#include "NMEAMsgType.h"
#include "NMEALine.h"

#include "StatsHolder.h"
#include "StatCounter.h"

#include "DataModelNode.h"
#include "DataModelUInt32Leaf.h"

#include "etl/set.h"

#include "freertos/FreeRTOS.h"
#include "freertos/message_buffer.h"

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
class NMEABridge : public TaskObject, NMEALineHandler, StatsHolder {
    private:
        static constexpr size_t MAX_BRIDGE_MSG_TYPES = 10;
        static constexpr size_t messageBufferLength = maxNMEALineLength * 5;
        static constexpr size_t stackSize = 4096;

        etl::set<NMEAMsgType, MAX_BRIDGE_MSG_TYPES> bridgedMsgTypes;
        const char *name;
        Interface &dstInterface;

        // Since we might be bridging from a 38400 baud interface to a 4800 baud one, we don't want
        // to directly send the message on the calling task as a burst of bridged messages might
        // cause a block, resulting in an input overrun on the faster interface. To avoid this
        // situation we enqueue the message to a separate bridge task which will handle the
        // potentially blocking write. This allows burst absorption greater than the length of
        // a typical UART TX buffer. Clearly the bridge configuration must make sense with the
        // resulting message stream being something that the output interface can handle.
        // (aka don't bridge AIS messages to 4800 baud).
        // We use freeRTOS message buffers for this as it allows for variable sized discrete
        // messages. We don't have to worry about its lack of mutual exclusion as we only have one
        // writer, the input task for the interface the bridge is configured on, and one reader,
        // the bridge task.
        MessageBufferHandle_t messagesBuffer;

        StatCounter bridgedMessages;
        uint32_t droppedMessages;
        uint32_t outputErrors;
        DataModelNode bridgeNode;
        DataModelUInt32Leaf bridgedMessagesLeaf;
        DataModelUInt32Leaf bridgedMessageRateLeaf;
        DataModelUInt32Leaf droppedMessageLeaf;
        DataModelUInt32Leaf outputErrorsLeaf;

        void buildBridgedMessageSet(const char *msgTypeList);
        virtual void handleLine(const NMEALine &inputLine) override;
        void parseMsgType(NMEAMsgType &msgType, const NMEALine &inputLine);
        virtual void task() override;
        virtual void exportStats(uint32_t msElapsed) override;

    public:
        NMEABridge(const char *name, const char *msgTypeList, NMEAInterface &srcInterface,
                   Interface &dstInterface, StatsManager &statsManager, DataModel &dataModel);
};

#endif // NMEA_BRIDGE_H
