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

#ifndef STALK_SOURCE_H
#define STALK_SOURCE_H

#include "SeaTalkParser.h"
#include "NMEALineSource.h"
#include "StatsHolder.h"
#include "StatCounter.h"
#include "DataModelUInt32Leaf.h"

#include <stdint.h>

class NMEALine;
class DataModelNode;
class StatsManager;

class STALKSource : public NMEALineSource, StatsHolder {
    private:
        SeaTalkParser seaTalkParser;
        StatCounter messagesCounter;
        DataModelUInt32Leaf messagesLeaf;
        DataModelUInt32Leaf messageRateLeaf;
        uint32_t illformedMessages;
        DataModelUInt32Leaf illformedMessagesLeaf;
        bool _lastMessageIllformed;

        virtual void exportStats(uint32_t msElapsed) override;
        virtual void handleLine(NMEALine &inputLine) override;
        bool parseLine(NMEALine &nmeaLine);

    public:
        STALKSource(const char *name, DataModelNode &interfaceNode, StatsManager &statsManager);
        bool lastMessageIllformed() const;
};

#endif // STALK_SOURCE_H
