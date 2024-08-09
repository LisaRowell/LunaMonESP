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

#ifndef SEATALK_PARSER_H
#define SEATALK_PARSER_H

#include "StatsHolder.h"
#include "StatCounter.h"
#include "DataModelNode.h"
#include "DataModelUInt32Leaf.h"

#include <stddef.h>
#include <stdint.h>

class SeaTalkLine;
class SeaTalkCommand;
class StatsManager;

class SeaTalkParser : StatsHolder {
    private:
        StatCounter commandsReceivedCounter;
        uint32_t ignoredCommands;
        uint32_t unknownCommands;
        uint32_t commandLengthErrors;
        uint32_t commandFormatErrors;
        DataModelNode seaTalkNode;
        DataModelNode receivedNode;
        DataModelUInt32Leaf commandsReceivedLeaf;
        DataModelUInt32Leaf commandseceiveRateLeaf;
        DataModelUInt32Leaf ignoredCommandsLeaf;
        DataModelUInt32Leaf unknownCommandsLeaf;
        DataModelUInt32Leaf commandLengthErrorsLeaf;
        DataModelUInt32Leaf commandFormatErrorsLeaf;

        void parseDepthBelowTransducer(const SeaTalkLine &seaTalkLine);
        void parseApparentWindAngle(const SeaTalkLine &seaTalkLine);
        void parseApparentWindSpeed(const SeaTalkLine &seaTalkLine);
        void parseSpeedThroughWaterV1(const SeaTalkLine &seaTalkLine);
        void parseWaterTemperatureV1(const SeaTalkLine &seaTalkLine);
        void parseSpeedThroughWaterV2(const SeaTalkLine &seaTalkLine);
        void parseWaterTemperatureV2(const SeaTalkLine &seaTalkLine);
        void parseSetLampIntensity(const SeaTalkLine &seaTalkLine);
        void ignoredCommand(const SeaTalkCommand &command, const SeaTalkLine &seaTalkLine);
        void unknownCommand(const SeaTalkCommand &command, const SeaTalkLine &seaTalkLine);
        bool checkLength(size_t expectedLength, const SeaTalkLine &seaTalkLine);
        bool checkAttribute(const SeaTalkLine &seaTalkLine, uint8_t expectedAttribute,
                            uint8_t mask = 0xff);
        virtual void exportStats(uint32_t msElapsed) override;

    public:
        SeaTalkParser(DataModelNode &interfaceNode, StatsManager &statsManager);
        void parseLine(const SeaTalkLine &seaTalkLine);
};

#endif // SEATALK_PARSER_H
