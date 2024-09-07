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

#include "freertos/FreeRTOS.h"
#include "freertos/message_buffer.h"

#include <stddef.h>
#include <stdint.h>

NMEABridge::NMEABridge(const char *name, const char *msgTypeList, NMEAInterface &srcInterface,
                       Interface &dstInterface, StatsManager &statsManager, DataModel &dataModel)
    : TaskObject::TaskObject("NMEA Bridge", LOGGER_LEVEL_DEBUG, stackSize),
      name(name),
      dstInterface(dstInterface),
      bridgedMessages(),
      droppedMessages(0),
      outputErrors(0),
      bridgeNode(name, &dataModel.sysNode()),
      bridgedMessagesLeaf("bridged", &bridgeNode),
      bridgedMessageRateLeaf("bridgedRate", &bridgeNode),
      droppedMessageLeaf("dropped", &bridgeNode),
      outputErrorsLeaf("outputErrors", &bridgeNode) {
    srcInterface.addLineHandler(*this);
    statsManager.addStatsHolder(*this);

    buildBridgedMessageSet(msgTypeList);

    if ((messagesBuffer = xMessageBufferCreate(messageBufferLength)) == nullptr) {
        logger << logErrorNMEABridge << "Failed to create message buffer for NMEA Bridge " << name
               << eol;
        errorExit();
    }
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
            logger << logErrorNMEABridge << "Unknown NMEA message type '" << msgTypeStrView
                    << "' in NMEA Bridge " << name << " list: " << msgTypeList << eol;
            errorExit();
        }

        if (bridgedMsgTypes.contains(msgType)) {
            logger << logWarnNMEABridge << "Duplicate NMEA message type '" << msgTypeStrView
                   << "' in NMEA Bridge " << name << " list: " << msgTypeList << eol;
        } else {
            bridgedMsgTypes.insert(msgType);
        }
    }
}

// Called from the input interface's task.
void NMEABridge::handleLine(const NMEALine &inputLine) {
    NMEAMsgType msgType;
    parseMsgType(msgType, inputLine);

    if (bridgedMsgTypes.contains(msgType)) {
        // Attempt to queue the message, but don't block. If the Message Buffer is full then we're
        // overrunning the output interface. Blocking here is just going to lead to input overruns.
        size_t queuedLength = xMessageBufferSend(messagesBuffer, inputLine.data(),
                                                 inputLine.length(), 0);
        if (queuedLength != 0) {
            bridgedMessages++;
            taskLogger() << logDebugNMEABridge << "Bridged NMEA " << msgType << " message to "
                         << dstInterface.name() << eol;
        } else {
            droppedMessages++;
            taskLogger() << logDebugNMEABridge << "Dropped NMEA " << msgType
                         << " message due to full bridge queue on bridge " << name << eol;
        }
    }
}

// Called from the input interface's task.
void NMEABridge::parseMsgType(NMEAMsgType &msgType, const NMEALine &inputLine) {
    NMEALineWalker lineWalker(inputLine);

    // At this point we can assume the line has either a leading '$' or '!' and has a valid
    // checksum.
    lineWalker.skipChar();

    etl::string_view tagView;
    if (!lineWalker.getWord(tagView)) {
        taskLogger() << logWarnNMEABridge << "NMEA message missing tag: " << inputLine << eol;
        return;
    }

    if (tagView.length() < 2) {
        taskLogger() << logWarnNMEABridge << "NMEA message missing talker: " << inputLine << eol;
        return;
    }
    tagView.remove_prefix(2);

    msgType.parse(tagView);
}

void NMEABridge::task() {
    while (true) {
        char message[maxNMEALineLength];
        size_t messageLength = xMessageBufferReceive(messagesBuffer, message, maxNMEALineLength,
                                                     portMAX_DELAY);
        if (messageLength > 0) {
            // Internally NMEA 0183 messages don't include the terminating CR,LF, so we need to add
            // it here before we output it.
            message[messageLength] = '\r';
            message[messageLength + 1] = '\n';
            message[messageLength + 2] = 0;
            bool sent = dstInterface.sendMessage(message, true);
            if (sent) {
                // Remove the CR,LF for the debug message
                message[messageLength] = '0';
                logger << logDebugNMEABridge << "Wrote NMEA message to " << dstInterface.name()
                       << ": " << message << eol;
            } else {
                outputErrors++;
                logger << logWarnNMEABridge << "Error sending NMEA bridged message to "
                       << dstInterface.name() << eol;
            }
        }
    }
}

void NMEABridge::exportStats(uint32_t msElapsed) {
    bridgedMessages.update(bridgedMessagesLeaf, bridgedMessageRateLeaf, msElapsed);
    droppedMessageLeaf = droppedMessages;
    outputErrorsLeaf = outputErrors;
}
