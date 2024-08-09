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

#include "WindBridge.h"

#include "NMEAMWVMessage.h"

#include "DataModel.h"
#include "DataModelNode.h"
#include "DataModelBoolLeaf.h"
#include "DataModelTenthsUInt16Leaf.h"

#include "StatCounter.h"

#include "Logger.h"

WindBridge::WindBridge(DataModel &dataModel, StatCounter &messagesBridgedCounter)
    : messagesBridgedCounter(messagesBridgedCounter),
      windNode("wind", &dataModel.rootNode()),
      apparentWindNode("apparent", &windNode),
      apparentWindAngleLeaf("angle", &apparentWindNode),
      apparentWindSpeedNode("speed", &apparentWindNode),
      apparentWindSpeedKnotsLeaf("knots", &apparentWindSpeedNode),
      apparentWindSpeedMPHLeaf("mph", &apparentWindSpeedNode),
      apparentWindSpeedKMPHLeaf("kmph", &apparentWindSpeedNode),
      trueWindNode("true", &windNode),
      trueWindAngleLeaf("angle", &trueWindNode),
      trueWindSpeedNode("speed", &trueWindNode),
      trueWindSpeedKnotsLeaf("knots", &trueWindSpeedNode),
      trueWindSpeedMPHLeaf("mph", &trueWindSpeedNode),
      trueWindSpeedKMPHLeaf("kmph", &trueWindSpeedNode),
      windValidLeaf("valid", &windNode)
{
}

void WindBridge::bridgeNMEAMWVMessage(const NMEAMWVMessage *message) {
    if (message->relativeIndicator.isRelative()) {
        message->windAngle.publish(apparentWindAngleLeaf);

        switch (message->windSpeedUnits) {
            case NMEASpeedUnits::KNOTS:
                message->windSpeed.publish(apparentWindSpeedKnotsLeaf);
                break;

            case NMEASpeedUnits::MILES_PER_HOUR:
                message->windSpeed.publish(apparentWindSpeedMPHLeaf);
                break;

            case NMEASpeedUnits::KILOMETERS_PER_HOUR:
                message->windSpeed.publish(apparentWindSpeedKMPHLeaf);
                break;

            default:
                logger() << logWarnDataModelBridge << "Unhandled wind speed units ("
                         << message->windSpeedUnits << ") in NMEA MWV message" << eol;
        }
    } else {
        message->windAngle.publish(trueWindAngleLeaf);

        switch (message->windSpeedUnits) {
            case NMEASpeedUnits::KNOTS:
                message->windSpeed.publish(trueWindSpeedKnotsLeaf);
                break;

            case NMEASpeedUnits::MILES_PER_HOUR:
                message->windSpeed.publish(trueWindSpeedMPHLeaf);
                break;

            case NMEASpeedUnits::KILOMETERS_PER_HOUR:
                message->windSpeed.publish(trueWindSpeedKMPHLeaf);
                break;

            default:
                logger() << logWarnDataModelBridge << "Unhandled wind speed units ("
                         << message->windSpeedUnits << ") in NMEA MWV message" << eol;
        }
    }

    message->dataValid.publish(windValidLeaf);

    messagesBridgedCounter++;
}
