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

#ifndef SEA_TALK_INTERFACE_H
#define SEA_TALK_INTERFACE_H

#include "SeaTalkMaster.h"
#include "StatsHolder.h"

#include "SeaTalkLine.h"

#include "DataModelNode.h"
#include "DataModelUInt32Leaf.h"
#include "StatCounter.h"

#include <stddef.h>
#include <stdint.h>

class SeaTalkParser;
class SeaTalkWriteTester;
class Interface;
class InstrumentData;
class StatsManager;
class SeaTalkNMEABridge;

class SeaTalkInterface : public SeaTalkMaster, StatsHolder {
    private:
        Interface &interface;
        SeaTalkLine inputLine;
        SeaTalkParser *parser;
        SeaTalkWriteTester *writeTester;
        StatCounter inputDatagramCounter;
        StatCounter collisionCounter;
        StatCounter outputDatagramCounter;
        uint32_t outputErrors;
        DataModelNode seaTalkNode;
        DataModelNode inputNode;
        DataModelUInt32Leaf inputDatagramsLeaf;
        DataModelUInt32Leaf inputDatagramsRateLeaf;
        DataModelUInt32Leaf collisionsLeaf;
        DataModelUInt32Leaf collisionRateLeaf;
        DataModelNode outputNode;
        DataModelUInt32Leaf outputDatagramsLeaf;
        DataModelUInt32Leaf outputDatagramsRateLeaf;
        DataModelUInt32Leaf outputErrorsLeaf;

        virtual void sendCommand(const SeaTalkLine &seaTalkLine);

    protected:
        virtual void exportStats(uint32_t msElapsed) override;

    public:
        SeaTalkInterface(Interface &interface, InstrumentData &instrumentData,
                         StatsManager &statsManager);
        void addBridge(SeaTalkNMEABridge *bridge);
        void start();
        void processBuffer(uint16_t *buffer, size_t length);
};

#endif // SEA_TALK_INTERFACE_H
