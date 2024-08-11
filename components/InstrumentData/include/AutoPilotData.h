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

#ifndef AUTO_PILOT_DATA_H
#define AUTO_PILOT_DATA_H

#include "InstrumentGroup.h"

#include "DataModelNode.h"
#include "DataModelTenthsInt16Leaf.h"
#include "DataModelTenthsUInt16Leaf.h"

class DataModel;
class StatsManager;

class AutoPilotData : public InstrumentGroup {
    private:
        DataModelNode autoPilotNode;
        DataModelNode autoPilotHeadingNode;
        DataModelNode autoPilotRudderNode;

    public:
        DataModelTenthsUInt16Leaf autoPilotHeadingSensorLeaf;
        DataModelTenthsInt16Leaf autoPilotHeadingDeviationLeaf;
        DataModelTenthsInt16Leaf autoPilotHeadingVariationLeaf;
        DataModelTenthsInt16Leaf autopilotRudderStarboardLeaf;
        DataModelTenthsInt16Leaf autopilotRudderPortLeaf;

        AutoPilotData(DataModel &dataModel, DataModelNode &instrumentDataNode,
                      StatsManager &statsManager);
};

#endif // AUTO_PILOT_DATA_H
