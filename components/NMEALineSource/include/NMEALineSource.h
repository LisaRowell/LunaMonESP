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

#ifndef NMEA_LINE_SOURCE_H
#define NMEA_LINE_SOURCE_H

#include "NMEALine.h"
#include "NMEALineHandler.h"
#include "NMEATalker.h"

#include "DataModelNode.h"
#include "DataModelStringLeaf.h"
#include "DataModelUInt32Leaf.h"

#include "StatsHolder.h"
#include "StatCounter.h"

#include "etl/vector.h"
#include "etl/set.h"

#include <stddef.h>
#include <stdint.h>

class StatsManager;
class NMEAMsgType;

class NMEALineSource : StatsHolder {
    private:
        static constexpr size_t MAX_LINE_HANDLERS = 4;
        static const size_t maxTalkers = 10;

        etl::vector<NMEALineHandler *, MAX_LINE_HANDLERS> lineHandlers;
        NMEALine inputLine;
        bool carriageReturnFound;
        etl::set<NMEATalker, maxTalkers> talkers;
        etl::set<NMEATalker, maxTalkers> filteredTalkers;
        StatCounter messagesCounter;
        uint32_t talkerFilteredMessages;
        etl::string<maxTalkers * 3> talkersBuffer;
        uint32_t badTagMessages;

        DataModelNode nmeaNode;
        DataModelNode _nmeaInputNode;
        DataModelUInt32Leaf messagesLeaf;
        DataModelUInt32Leaf messageRateLeaf;
        DataModelStringLeaf talkersLeaf;
        DataModelUInt32Leaf talkerFilteredMessagesLeaf;
        DataModelUInt32Leaf badTagsMessagesLeaf;

        void buildFilteredTalkersSet(const char *filteredTalkersList);
        bool scanForCarriageReturn(size_t &carriageReturnPos, const char *buffer,
                                   size_t &bufferPos, size_t &remaining);
        bool processBufferToEndOfLine(const char *buffer, size_t &bufferPos, size_t &remaining);
        void lineCompleted();
        bool parseTag(NMEATalker &talker, NMEAMsgType &msgType);
        void newTalkerSeen(const NMEATalker &talker);
        void messageFilteredByTalker(const NMEATalker &talker);
        void handleLine(const NMEATalker &talker, const NMEAMsgType &msgType);
        virtual void exportStats(uint32_t msElapsed) override;

    protected:
        void processBuffer(const char *buffer, size_t length);
        DataModelNode &nmeaInputNode();

    public:
        NMEALineSource(DataModelNode &interfaceNode, const char *filteredTalkersList,
                       StatsManager &statsManager);
        void addLineHandler(NMEALineHandler &lineHandler);
        void sourceReset();
};

#endif // NMEA_LINE_SOURCE_H
