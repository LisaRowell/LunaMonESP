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

#include "NMEAWaterBridge.h"

#include "NMEAMTWMessage.h"
#include "NMEATemperatureUnits.h"
#include "NMEATenthsInt16.h"

#include "DataModel.h"
#include "DataModelNode.h"
#include "DataModelTenthsInt16Leaf.h"

#include "TenthsInt16.h"

#include "StatCounter.h"

NMEAWaterBridge::NMEAWaterBridge(DataModel &dataModel, StatCounter &messagesBridgedCounter)
    : messagesBridgedCounter(messagesBridgedCounter),
      waterNode("water", &dataModel.rootNode()),
      waterTemperatureNode("temperature", &waterNode),
      waterTemperatureCelsiusLeaf("celsius", &waterTemperatureNode),
      waterTemperatureFahrenheitLeaf("fahrenheit", &waterTemperatureNode) {
}

void NMEAWaterBridge::bridgeNMEAMTWMessage(const NMEAMTWMessage *message) {
    switch (message->waterTemperatureUnits) {
        case NMEATemperatureUnits::CELSIUS:
            message->waterTemperature.publish(waterTemperatureCelsiusLeaf);
            break;

        case NMEATemperatureUnits::FAHRENHEIT:
            message->waterTemperature.publish(waterTemperatureFahrenheitLeaf);
            break;

        default:
            logger() << logWarnNMEADataModelBridge << "Unhandled water temperature units ("
                     << message->waterTemperatureUnits << ") in NMEA MTW message" << eol;
    }

    messagesBridgedCounter++;
}
