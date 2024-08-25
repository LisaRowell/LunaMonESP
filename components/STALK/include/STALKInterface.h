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

#ifndef STALK_INTERFACE_H
#define STALK_INTERFACE_H

#include "SeaTalkParser.h"
#include "SeaTalkMaster.h"
#include "NMEALineSource.h"
#include "NMEALineHandler.h"
#include "StatsHolder.h"
#include "StatCounter.h"
#include "DataModelNode.h"
#include "DataModelUInt32Leaf.h"

#include <stdint.h>

class NMEALine;
class Interface;
class InstrumentData;
class StatsManager;

class STALKInterface : public NMEALineSource, NMEALineHandler, public SeaTalkMaster, StatsHolder {
    private:
        Interface &interface;
        SeaTalkParser seaTalkParser;
        StatCounter messagesCounter;
        uint32_t illformedMessages;
        bool _lastMessageIllformed;
        DataModelNode stalkNode;
        DataModelUInt32Leaf messagesLeaf;
        DataModelUInt32Leaf messageRateLeaf;
        DataModelUInt32Leaf illformedMessagesLeaf;

        virtual void exportStats(uint32_t msElapsed) override;
        void handleLine(NMEALine &inputLine);
        bool parseLine(NMEALine &nmeaLine);
        void parseDatagramMessage(NMEALine &nmeaLine);
        void parsePropritoryMessage(NMEALine &nmeaLine);

        virtual void sendCommand(const SeaTalkLine &seaTalkLine) override;

    public:
        STALKInterface(Interface &interface, InstrumentData &instrumentData,
                       StatsManager &statsManager);
        bool lastMessageIllformed() const;
};

#endif // STALK_INTERFACE_H
