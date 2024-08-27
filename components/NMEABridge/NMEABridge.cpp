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

#include "NMEABridge.h"
#include "NMEAMsgType.h"
#include "NMEAInterface.h"
#include "NMEALine.h"
#include "NMEALineWalker.h"
#include "NMEALineHandler.h"
#include "Interface.h"

#include "StatsManager.h"

#include "DataModel.h"
#include "DataModelNode.h"
#include "DataModelUInt32Leaf.h"

#include "Logger.h"
#include "Error.h"

#include "etl/set.h"
#include "etl/string_view.h"

#include <stddef.h>
#include <stdint.h>

NMEABridge::NMEABridge(const char *name, const char *msgTypeList, NMEAInterface &srcInterface,
                       Interface &dstInterface, StatsManager &statsManager, DataModel &dataModel)
    : name(name),
      dstInterface(dstInterface),
      bridgedMessages(),
      droppedMessages(0),
      bridgeNode(name, &dataModel.sysNode()),
      bridgedMessagesLeaf("bridged", &bridgeNode),
      bridgedMessageRateLeaf("bridgedRate", &bridgeNode),
      droppedMessageLeaf("dropped", &bridgeNode) {
    srcInterface.addLineHandler(*this);
    statsManager.addStatsHolder(*this);

    buildBridgedMessageSet(msgTypeList);
}

void NMEABridge::buildBridgedMessageSet(const char *msgTypeList) {
    etl::string_view msgTypeListView(msgTypeList);

    while (!msgTypeListView.empty()) {
        etl::string_view msgTypeStrView;

        size_t commaPos = msgTypeListView.find(',');
        if (commaPos == msgTypeListView.npos) {
            msgTypeStrView.assign(msgTypeListView.begin(), msgTypeListView.end());
            msgTypeListView.remove_prefix(msgTypeListView.size());
        } else {
            msgTypeStrView.assign(msgTypeListView.begin(), msgTypeListView.begin() + commaPos);
            msgTypeListView.remove_prefix(commaPos + 1);
        }

        NMEAMsgType msgType(msgTypeStrView);
        if (msgType == NMEAMsgType::UNKNOWN) {
            logger() << logErrorNMEABridge << "Unknown NMEA message type '" << msgTypeStrView
                     << "' in NMEA Bridge " << name << " list: " << msgTypeList << eol;
            errorExit();
        }

        if (bridgedMsgTypes.contains(msgType)) {
            logger() << logWarnNMEABridge << "Duplicate NMEA message type '" << msgTypeStrView
                     << "' in NMEA Bridge " << name << " list: " << msgTypeList << eol;
        } else {
            bridgedMsgTypes.insert(msgType);
        }
    }
}

void NMEABridge::handleLine(const NMEALine &inputLine) {
    NMEAMsgType msgType;
    parseMsgType(msgType, inputLine);

    if (bridgedMsgTypes.contains(msgType)) {
        bool sent = dstInterface.sendMessage(inputLine.contents(), false);
        if (sent) {
            bridgedMessages++;
            logger() << logDebugNMEABridge << "Bridged NMEA " << msgType << " message to "
                     << dstInterface.name() << eol;
        } else {
            droppedMessages++;
            logger() << logDebugNMEABridge << "Dropped NMEA " << msgType
                     << " message due to full output interface " << dstInterface.name() << eol;
        }
    }
}

void NMEABridge::parseMsgType(NMEAMsgType &msgType, const NMEALine &inputLine) {
    NMEALineWalker lineWalker(inputLine);

    // At this point we can assume the line has either a leading '$' or '!' and has a valid
    // checksum.
    lineWalker.skipChar();

    etl::string_view tagView;
    if (!lineWalker.getWord(tagView)) {
        logger() << logWarnNMEABridge << "NMEA message missing tag: " << inputLine << eol;
        return;
    }

    if (tagView.length() < 2) {
        logger() << logWarnNMEABridge << "NMEA message missing talker: " << inputLine << eol;
        return;
    }
    tagView.remove_prefix(2);

    msgType.parse(tagView);
}

void NMEABridge::exportStats(uint32_t msElapsed) {
    bridgedMessages.update(bridgedMessagesLeaf, bridgedMessageRateLeaf, msElapsed);
    droppedMessageLeaf = droppedMessages;
}
