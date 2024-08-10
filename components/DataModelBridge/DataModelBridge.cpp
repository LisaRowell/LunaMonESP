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

#include "DataModelBridge.h"
#include "AutoPilotBridge.h"
#include "GPSBridge.h"
#include "WaterBridge.h"
#include "WindBridge.h"

#include "NMEADBKMessage.h"
#include "NMEADBSMessage.h"
#include "NMEADBTMessage.h"
#include "NMEAGGAMessage.h"
#include "NMEAGLLMessage.h"
#include "NMEAGSAMessage.h"
#include "NMEAGSTMessage.h"
#include "NMEAHDGMessage.h"
#include "NMEAMTWMessage.h"
#include "NMEAMWVMessage.h"
#include "NMEARMCMessage.h"
#include "NMEARSAMessage.h"
#include "NMEAVHWMessage.h"
#include "NMEAVTGMessage.h"
#include "NMEATXTMessage.h"

#include "StatCounter.h"

#include "StatsManager.h"

#include "DataModel.h"
#include "DataModelNode.h"
#include "DataModelUInt32Leaf.h"


DataModelBridge::DataModelBridge(DataModel &dataModel, StatsManager &statsManager)
    : autoPilotBridge(dataModel, messagesBridgedCounter),
      gpsBridge(dataModel, messagesBridgedCounter),
      waterBridge(dataModel, messagesBridgedCounter),
      windBridge(dataModel, messagesBridgedCounter),
      dataModelBridgeNode("dataModelBridge", &dataModel.sysNode()),
      messagesBridgedLeaf("messages", &dataModelBridgeNode),
      messagesBridgedRateLeaf("messageRate", &dataModelBridgeNode) {
    statsManager.addStatsHolder(*this);
}

void DataModelBridge::processMessage(const NMEAMessage *message) {
    const NMEAMsgType msgType = message->type();
    switch (msgType) {
        case NMEAMsgType::DBK:
            waterBridge.bridgeNMEADBKMessage((NMEADBKMessage *)message);
            break;

        case NMEAMsgType::DBS:
            waterBridge.bridgeNMEADBSMessage((NMEADBSMessage *)message);
            break;

        case NMEAMsgType::DBT:
            waterBridge.bridgeNMEADBTMessage((NMEADBTMessage *)message);
            break;

        case NMEAMsgType::DPT:
            waterBridge.bridgeNMEADPTMessage((NMEADPTMessage *)message);
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

        case NMEAMsgType::HDG:
            autoPilotBridge.bridgeNMEAHDGMessage((NMEAHDGMessage *)message);
            break;

        case NMEAMsgType::MTW:
            waterBridge.bridgeNMEAMTWMessage((NMEAMTWMessage *)message);
            break;

        case NMEAMsgType::MWV:
            windBridge.bridgeNMEAMWVMessage((NMEAMWVMessage *)message);
            break;

        case NMEAMsgType::RMC:
            gpsBridge.bridgeNMEARMCMessage((NMEARMCMessage *)message);
            break;

        case NMEAMsgType::RSA:
            autoPilotBridge.bridgeNMEARSAMessage((NMEARSAMessage *)message);
            break;

        case NMEAMsgType::VHW:
            waterBridge.bridgeNMEAVHWMessage((NMEAVHWMessage *)message);
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
            taskLogger() << logDebugDataModelBridge << "Ignoring " << message->source() << " "
                         << msgType << " message in NMEA->Data Model Bridge" << eol;
            break;

        default:
            taskLogger() << logWarnDataModelBridge << "Unhandled " << message->source() << " "
                         << msgType << " message in NMEA->Data Model Bridge" << eol;
    }
}

void DataModelBridge::logTXTMessage(NMEATXTMessage *message) {
    // While we don't have a way of making TXT messages available in the DataModel, we output them
    // as info log messages here to aid in debugging. Later we could trade some memory for a way to
    // see these.
    taskLogger() << logNotifyDataModelBridge << message->source() << " TXT ("
                 << message->sentenceNumber << "/" << message->totalSentences << ") id "
                 << message->textIdentifier << ": " << message->text << eol;
}

void DataModelBridge::exportStats(uint32_t msElapsed) {
    messagesBridgedCounter.update(messagesBridgedLeaf, messagesBridgedRateLeaf, msElapsed);
}
