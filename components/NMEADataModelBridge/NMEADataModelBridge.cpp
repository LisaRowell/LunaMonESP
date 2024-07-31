/*
 * This file is part of LunaMon (https://github.com/LisaRowell/LunaMonESP)
 * Copyright (C) 2021-2024 Lisa Rowell
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

#include "NMEADataModelBridge.h"
#include "NMEADepthBridge.h"

#include "NMEADBKMessage.h"
#include "NMEADBSMessage.h"
#include "NMEADBTMessage.h"
#include "NMEAGGAMessage.h"
#include "NMEAGLLMessage.h"
#include "NMEAGSAMessage.h"
#include "NMEAGSTMessage.h"
#include "NMEARMCMessage.h"
#include "NMEAVTGMessage.h"
#include "NMEATXTMessage.h"

#include "StatCounter.h"

#include "StatsManager.h"

#include "DataModel.h"
#include "DataModelNode.h"
#include "DataModelUInt32Leaf.h"


NMEADataModelBridge::NMEADataModelBridge(DataModel &dataModel, StatsManager &statsManager)
    : depthBridge(dataModel, messagesBridgedCounter),
      gpsBridge(dataModel, messagesBridgedCounter),
      windBridge(dataModel, messagesBridgedCounter),
      nmeaDataModelBridgeNode("nmeaDataModelBridge", &dataModel.sysNode()),
      messagesBridgedLeaf("messages", &nmeaDataModelBridgeNode),
      messagesBridgedRateLeaf("messageRate", &nmeaDataModelBridgeNode) {
    statsManager.addStatsHolder(*this);
}

void NMEADataModelBridge::processMessage(const NMEAMessage *message) {
    const NMEAMsgType msgType = message->type();
    switch (msgType) {
        case NMEAMsgType::DBK:
            depthBridge.bridgeNMEADBKMessage((NMEADBKMessage *)message);
            break;

        case NMEAMsgType::DBS:
            depthBridge.bridgeNMEADBSMessage((NMEADBSMessage *)message);
            break;

        case NMEAMsgType::DBT:
            depthBridge.bridgeNMEADBTMessage((NMEADBTMessage *)message);
            break;

        case NMEAMsgType::DPT:
            depthBridge.bridgeNMEADPTMessage((NMEADPTMessage *)message);
            break;

        case NMEAMsgType::GGA:
            gpsBridge.bridgeNMEAGGAMessage((NMEAGGAMessage *)message);
            break;

        case NMEAMsgType::GLL:
            gpsBridge.bridgeNMEAGLLMessage((NMEAGLLMessage *)message);
            break;

        case NMEAMsgType::GSA:
            gpsBridge.bridgeNMEAGSAMessage((NMEAGSAMessage *)message);
            break;

        case NMEAMsgType::GST:
            gpsBridge.bridgeNMEAGSTMessage((NMEAGSTMessage *)message);
            break;

        case NMEAMsgType::MWV:
            windBridge.bridgeNMEAMWVMessage((NMEAMWVMessage *)message);
            break;

        case NMEAMsgType::RMC:
            gpsBridge.bridgeNMEARMCMessage((NMEARMCMessage *)message);
            break;

        case NMEAMsgType::VTG:
            gpsBridge.bridgeNMEAVTGMessage((NMEAVTGMessage *)message);
            break;

        case NMEAMsgType::TXT:
            logTXTMessage((NMEATXTMessage *)message);
            break;

        case NMEAMsgType::GSV:
        case NMEAMsgType::VDM:
        case NMEAMsgType::VDO:
            // Currently not output to clients.
            taskLogger() << logDebugNMEADataModelBridge << "Ignoring " << message->source() << " "
                         << msgType << " message in NMEA->Data Model Bridge" << eol;
            break;

        default:
            taskLogger() << logWarnNMEADataModelBridge << "Unhandled " << message->source() << " "
                         << msgType << " message in NMEA->Data Model Bridge" << eol;
    }
}

void NMEADataModelBridge::logTXTMessage(NMEATXTMessage *message) {
    // While we don't have a way of making TXT messages available in the DataModel, we output them
    // as info log messages here to aid in debugging. Later we could trade some memory for a way to
    // see these.
    taskLogger() << logNotifyNMEADataModelBridge << message->source() << " TXT ("
                 << message->sentenceNumber << "/" << message->totalSentences << ") id "
                 << message->textIdentifier << ": " << message->text << eol;
}

void NMEADataModelBridge::exportStats(uint32_t msElapsed) {
    messagesBridgedCounter.update(messagesBridgedLeaf, messagesBridgedRateLeaf, msElapsed);
}
