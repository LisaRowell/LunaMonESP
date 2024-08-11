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

#include "WindData.h"
#include "InstrumentGroup.h"

#include "DataModel.h"
#include "DataModelNode.h"
#include "DataModelBoolLeaf.h"
#include "DataModelTenthsUInt16Leaf.h"

WindData::WindData(DataModel &dataModel, DataModelNode &instrumentDataNode,
                   StatsManager &statsManager)
    : InstrumentGroup("Wind", "wind", instrumentDataNode, statsManager),
      windNode("wind", &dataModel.rootNode()),
      apparentWindNode("apparent", &windNode),
      apparentWindSpeedNode("speed", &apparentWindNode),
      trueWindNode("true", &windNode),
      trueWindSpeedNode("speed", &trueWindNode),
      apparentWindAngleLeaf("angle", &apparentWindNode),
      apparentWindSpeedKnotsLeaf("knots", &apparentWindSpeedNode),
      apparentWindSpeedMPHLeaf("mph", &apparentWindSpeedNode),
      apparentWindSpeedKMPHLeaf("kmph", &apparentWindSpeedNode),
      apparentWindSpeedMPSLeaf("mps", &apparentWindSpeedNode),
      trueWindAngleLeaf("angle", &trueWindNode),
      trueWindSpeedKnotsLeaf("knots", &trueWindSpeedNode),
      trueWindSpeedMPHLeaf("mph", &trueWindSpeedNode),
      trueWindSpeedKMPHLeaf("kmph", &trueWindSpeedNode),
      windValidLeaf("valid", &windNode) {
}
