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

#ifndef NMEA_SOURCE_H
#define NMEA_SOURCE_H

#include "NMEALine.h"

#include "StatCounter.h"

#include "StatsHolder.h"

#include "etl/vector.h"

#include <stddef.h>

class NMEAMessageHandler;
class DataModelUInt32Leaf;
class StatsManager;

class NMEASource : StatsHolder {
    private:
        char buffer[maxNMEALineLength];
        size_t bufferPos;
        size_t remaining;
        bool carriageReturnFound;
        NMEALine inputLine;
        static const size_t maxMessageHandlers = 5;
        etl::vector<NMEAMessageHandler *, maxMessageHandlers> messageHandlers;

        StatCounter messagesCounter;
        DataModelUInt32Leaf &messagesLeaf;
        DataModelUInt32Leaf &messageRateLeaf;

        bool scanForCarriageReturn(size_t &carriageReturnPos);
        bool readAvailableInput(int sock);
        bool processBuffer();
        void lineCompleted();
        virtual void exportStats(uint32_t msElapsed) override;

    protected:
        void processNMEAStream(int sock);

    public:
        NMEASource(DataModelUInt32Leaf &messagesLeaf, DataModelUInt32Leaf &messageRateLeaf,
                   StatsManager &statsManager);
        void addMessageHandler(NMEAMessageHandler &messageHandler);
};

#endif
