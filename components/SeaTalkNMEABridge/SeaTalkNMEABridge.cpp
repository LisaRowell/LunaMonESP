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

#include "TenthsUInt16.h"

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

void SeaTalkNMEABridge::bridgeDBTMessage(const TenthsUInt16 &depthFeet) {
    etl::string<10> depthFeetStr;
    depthFeet.toString(depthFeetStr);

    etl::string<maxNMEALineLength> message;
    etl::string_stream messageStream(message);
    messageStream << "$" << talkerCode << "DBT" << "," << depthFeetStr << ",f,,M,,F";

    bridgeMessage("DBT", message);
}

void SeaTalkNMEABridge::bridgeHDMMessage(uint16_t heading) {
    etl::string<maxNMEALineLength> message;
    etl::string_stream messageStream(message);
    messageStream << "$" << talkerCode << "HDM" << "," << etl::setprecision(1) << heading << ",M";

    bridgeMessage("HDM", message);
}

void SeaTalkNMEABridge::bridgeMWVMessage(const TenthsUInt16 &windAngle, bool windAngleValid,
                                         const TenthsUInt16 &windSpeedKN, bool windSpeedValid) {
    etl::string<10> windAngleStr;
    windAngle.toString(windAngleStr);
    etl::string<10> windSpeedKNStr;
    windSpeedKN.toString(windSpeedKNStr);

    etl::string<maxNMEALineLength> message;
    etl::string_stream messageStream(message);
    messageStream << "$" << talkerCode << "MWV" << ",";
    if (windAngleValid) {
        messageStream << windAngleStr;
    }
    messageStream << ",R,";
    if (windSpeedValid) {
        messageStream << windSpeedKNStr;
    }
    messageStream << ",K," << validityCode(windAngleValid | windSpeedValid);

    bridgeMessage("MWV", message);
}

void SeaTalkNMEABridge::bridgeRSAMessage(int8_t stbdRudderPos, bool stbdRudderPosValid,
                                         int8_t portRudderPos, bool portRudderPosValid) {
    etl::string<maxNMEALineLength> message;
    etl::string_stream messageStream(message);
    messageStream << "$" << talkerCode << "RSA" << "," << etl::setprecision(1)
                  << stbdRudderPos << "," << validityCode(stbdRudderPosValid) << ","
                  << portRudderPos << "," << validityCode(portRudderPosValid);

    bridgeMessage("RSA", message);
}

void SeaTalkNMEABridge::bridgeMessage(const char *typeCode, const etl::istring &message) {
    NMEALine nmeaLine(message);
    nmeaLine.appendChecksum();

    logger() << logWarnSeaTalkNMEABridge << "Bridging from SeaTalk: " << nmeaLine << eol;

    destination.handleLine(nmeaLine);
    bridgedMessages++;
}

const char *SeaTalkNMEABridge::validityCode(bool valid) {
    if (valid) {
        return "A";
    } else {
        return "V";  // How V became meaning invalid I'll never know...
    }
}

void SeaTalkNMEABridge::exportStats(uint32_t msElapsed) {
    bridgedMessages.update(bridgedMessagesLeaf, bridgedMessageRateLeaf, msElapsed);
}
