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

#include "StatCounter.h"

#include "DataModel.h"
#include "DataModelNode.h"
#include "DataModelUInt32Leaf.h"


NMEADataModelBridge::NMEADataModelBridge(DataModel &dataModel)
    : depthBridge(dataModel, messagesBridgedCounter),
      gpsBridge(dataModel, messagesBridgedCounter),
      nmeaDataModelBridgeNode("nmeaDataModelBridge", &dataModel.sysNode()),
      messagesBridgedLeaf("messages", &nmeaDataModelBridgeNode),
      messagesBridgedRateLeaf("messageRate", &nmeaDataModelBridgeNode) {
}

void NMEADataModelBridge::processMessage(NMEAMessage *message) {
    if (0) {
    const NMEAMsgType msgType = message->type();
    switch (msgType) {
        case NMEA_MSG_TYPE_DBK:
            depthBridge.bridgeNMEADBKMessage((NMEADBKMessage *)message);
            break;

        case NMEA_MSG_TYPE_DBS:
            depthBridge.bridgeNMEADBSMessage((NMEADBSMessage *)message);
            break;

        case NMEA_MSG_TYPE_DBT:
            depthBridge.bridgeNMEADBTMessage((NMEADBTMessage *)message);
            break;

        case NMEA_MSG_TYPE_GGA:
            gpsBridge.bridgeNMEAGGAMessage((NMEAGGAMessage *)message);
            break;

        case NMEA_MSG_TYPE_GLL:
            gpsBridge.bridgeNMEAGLLMessage((NMEAGLLMessage *)message);
            break;

        case NMEA_MSG_TYPE_GSA:
            gpsBridge.bridgeNMEAGSAMessage((NMEAGSAMessage *)message);
            break;

        case NMEA_MSG_TYPE_GST:
            gpsBridge.bridgeNMEAGSTMessage((NMEAGSTMessage *)message);
            break;

        case NMEA_MSG_TYPE_RMC:
            gpsBridge.bridgeNMEARMCMessage((NMEARMCMessage *)message);
            break;

        case NMEA_MSG_TYPE_VTG:
            gpsBridge.bridgeNMEAVTGMessage((NMEAVTGMessage *)message);
            break;

        case NMEA_MSG_TYPE_GSV:
        case NMEA_MSG_TYPE_TXT:
        case NMEA_MSG_TYPE_VDM:
        case NMEA_MSG_TYPE_VDO:
            // Currently not output to clients.
            taskLogger() << logDebugNMEADataModelBridge << "Ignoring " << message->source() << " "
                         << nmeaMsgTypeName(msgType) << " message in NMEA->Data Model Bridge"
                        << eol;
            break;

        default:
            taskLogger() << logWarnNMEADataModelBridge << "Unhandled " << message->source() << " "
                         << nmeaMsgTypeName(msgType) << " message in NMEA->Data Model Bridge"
                         << eol;
    }
    }
}

void NMEADataModelBridge::exportStats(uint32_t msElapsed) {
    messagesBridgedCounter.update(messagesBridgedLeaf, messagesBridgedRateLeaf, msElapsed);
}
