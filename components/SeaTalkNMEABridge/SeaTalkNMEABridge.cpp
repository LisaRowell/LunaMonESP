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

#include "SeaTalkNMEABridge.h"

#include "NMEALine.h"
#include "NMEALineHandler.h"
#include "StatCounter.h"
#include "StatsManager.h"

#include "DataModel.h"
#include "DataModelNode.h"
#include "DataModelUInt32Leaf.h"

#include "Logger.h"

#include "etl/string.h"
#include "etl/string_stream.h"

#include <stdint.h>
#include <string.h>

SeaTalkNMEABridge::SeaTalkNMEABridge(const char *name, const char *label, const char *talkerCode,
                                     NMEALineHandler &destination, StatsManager &statsManager,
                                     DataModel &dataModel)
    : talkerCode(talkerCode),
      destination(destination),
      bridgedMessages(),
      bridgeNode(name, &dataModel.sysNode()),
      labelLeaf("label", &bridgeNode, labelBuffer),
      bridgedMessagesLeaf("bridged", &bridgeNode),
      bridgedMessageRateLeaf("bridgedRate", &bridgeNode) {
    statsManager.addStatsHolder(*this);

    if (strlen(talkerCode) != 2) {
        logger() << logWarnSeaTalkNMEABridge << "Bad SeaTalk NMEA Bridge Talker Code '"
                 << talkerCode << "'" << eol;
    }

    labelLeaf = label;
}

void SeaTalkNMEABridge::bridgeHDMMessage(uint16_t heading) {
    etl::string<maxNMEALineLength> message;
    etl::string_stream messageStream(message);
    messageStream << "$" << talkerCode << "HDM" << "," << etl::setprecision(1) << heading << ",M";

    bridgeMessage("HDM", message);
}

void SeaTalkNMEABridge::bridgeMessage(const char *typeCode, const etl::istring &message) {
    NMEALine nmeaLine(message);
    nmeaLine.appendChecksum();

    logger() << logWarnSeaTalkNMEABridge << "Bridging from SeaTalk: " << nmeaLine << eol;

    destination.handleLine(nmeaLine);
    bridgedMessages++;
}

void SeaTalkNMEABridge::exportStats(uint32_t msElapsed) {
    bridgedMessages.update(bridgedMessagesLeaf, bridgedMessageRateLeaf, msElapsed);
}
