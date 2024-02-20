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

#include "MQTTSubscribeMessage.h"
#include "MQTTMessage.h"
#include "MQTTString.h"

#include "Logger.h"

#include <stdint.h>

MQTTSubscribeMessage::MQTTSubscribeMessage(MQTTMessage const &message) : MQTTMessage(message) {
}

bool MQTTSubscribeMessage::parse() {
    if (fixedHeaderFlags() != 0x2) {
        taskLogger() << logWarnMQTT
                     << "Received MQTT SUBSCRIBE message with invalid Fixed Header Flags" << eol;
        return false;
    }

    if (remainingLength < sizeof(MQTTSubscribeVariableHeader)) {
        taskLogger() << logWarnMQTT
                     << "Received MQTT SUBSCRIBE message with a size too small for the "
                        "Variable Header." << eol;
        return false;
    }

    variableHeader = (MQTTSubscribeVariableHeader *)variableHeaderStart;
    const uint32_t bytesAfterVariableHdr = remainingLength - sizeof(MQTTSubscribeVariableHeader);

    if (packetId() == 0) {
        taskLogger() << logWarnMQTT
                     << "Received MQTT SUBSCRIBE message with zero Packet Indentifier." << eol;
        return false;
    }

    if (bytesAfterVariableHdr == 0) {
        taskLogger() << logWarnMQTT << "Received MQTT SUBSCRIBE message without any Topic Filters."
                     << eol;
        return false;
    }

    payloadStart = (uint8_t *)variableHeaderStart + sizeof(MQTTSubscribeVariableHeader);
    uint8_t *payloadPos = payloadStart;
    uint32_t payloadBytesRemaining = bytesAfterVariableHdr;

    // Run through the topic filters making sure the message is properly formed.
    topicFilters = 0;
    do {
        MQTTString *topicFilterStr;
        if (!parseString(topicFilterStr, payloadPos, payloadBytesRemaining)) {
            taskLogger() << logWarnMQTT
                         << "MQTT SUBSCRIBE message with payload too small for its Topic Filter"
                         << eol;
            return false;
        }

        if (topicFilterStr->length() == 0) {
            taskLogger() << logWarnMQTT << "MQTT SUBSCRIBE message with zero length Topic Filter"
                         << eol;
            return false;
        }

        if (payloadBytesRemaining < 1) {
            taskLogger() << logWarnMQTT
                         << "MQTT SUBSCRIBE message with a Topic Filter missing its max QoS" << eol;
            return false;
        }
        uint8_t maxQoS = *payloadPos++;
        payloadBytesRemaining--;

        if (maxQoS > 2) {
            taskLogger() << logWarnMQTT << "MQTT SUBSCRIBE message with illegal max QoS (" << Hex
                         << maxQoS << ")" << eol;
            return false;
        }

        topicFilters++;
    } while (payloadBytesRemaining);

    topicFiltersPos = payloadStart;
    topicFiltersReturned = 0;

    return true;
}

bool MQTTSubscribeMessage::getTopicFilter(MQTTString * &topicFilter, uint8_t &maxQoS) {
    if (topicFiltersReturned == topicFilters) {
        return false;
    }

    grabString(topicFilter, topicFiltersPos);
    maxQoS = *topicFiltersPos++;

    topicFiltersReturned++;

    return true;
}

uint16_t MQTTSubscribeMessage::packetId() const {
    return variableHeader->packetIdMSB * 256 + variableHeader->packetIdLSB;
}

unsigned MQTTSubscribeMessage::numTopicFilters() const {
    return topicFilters;
}
