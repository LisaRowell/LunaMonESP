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
#include "InstrumentData.h"
#include "WindData.h"

#include "NMEAMWVMessage.h"

#include "DataModel.h"
#include "DataModelNode.h"
#include "DataModelBoolLeaf.h"
#include "DataModelTenthsUInt16Leaf.h"

#include "StatCounter.h"

#include "Logger.h"

WindBridge::WindBridge(InstrumentData &instrumentData)
    : windData(instrumentData.windData()) {
}

void WindBridge::bridgeNMEAMWVMessage(const NMEAMWVMessage *message) {
    windData.beginUpdates();

    if (message->relativeIndicator.isRelative()) {
        message->windAngle.publish(windData.apparentWindAngleLeaf);

        switch (message->windSpeedUnits) {
            case NMEASpeedUnits::KNOTS:
                message->windSpeed.publish(windData.apparentWindSpeedKnotsLeaf);
                break;

            case NMEASpeedUnits::MILES_PER_HOUR:
                message->windSpeed.publish(windData.apparentWindSpeedMPHLeaf);
                break;

            case NMEASpeedUnits::KILOMETERS_PER_HOUR:
                message->windSpeed.publish(windData.apparentWindSpeedKMPHLeaf);
                break;

            default:
                logger() << logWarnDataModelBridge << "Unhandled wind speed units ("
                         << message->windSpeedUnits << ") in NMEA MWV message" << eol;
        }
    } else {
        message->windAngle.publish(windData.trueWindAngleLeaf);

        switch (message->windSpeedUnits) {
            case NMEASpeedUnits::KNOTS:
                message->windSpeed.publish(windData.trueWindSpeedKnotsLeaf);
                break;

            case NMEASpeedUnits::MILES_PER_HOUR:
                message->windSpeed.publish(windData.trueWindSpeedMPHLeaf);
                break;

            case NMEASpeedUnits::KILOMETERS_PER_HOUR:
                message->windSpeed.publish(windData.trueWindSpeedKMPHLeaf);
                break;

            default:
                logger() << logWarnDataModelBridge << "Unhandled wind speed units ("
                         << message->windSpeedUnits << ") in NMEA MWV message" << eol;
        }
    }

    message->dataValid.publish(windData.windValidLeaf);

    windData.endUpdates();
}
