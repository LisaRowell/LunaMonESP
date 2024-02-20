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

#include "MQTTUnsubscribeMessage.h"
#include "MQTTMessage.h"
#include "MQTTString.h"

#include "Logger.h"

#include <stdint.h>

MQTTUnsubscribeMessage::MQTTUnsubscribeMessage(MQTTMessage const &message) : MQTTMessage(message) {
}

bool MQTTUnsubscribeMessage::parse() {
    if (fixedHeaderFlags() != 0x2) {
        taskLogger() << logWarnMQTT
                     << "Received MQTT UNSUBSCRIBE message with invalid Fixed Header Flags" << eol;
        return false;
    }

    if (remainingLength < sizeof(MQTTUnsubscribeVariableHeader)) {
        taskLogger() << logWarnMQTT
                     << "Received MQTT UNSUBSCRIBE message with a size too small for the "
                        "Variable Header." << eol;
        return false;
    }

    variableHeader = (MQTTUnsubscribeVariableHeader *)variableHeaderStart;
    const uint32_t bytesAfterVariableHdr = remainingLength - sizeof(MQTTUnsubscribeVariableHeader);

    if (packetId() == 0) {
        taskLogger() << logWarnMQTT
                     << "Received MQTT UNSUBSCRIBE message with zero Packet Indentifier." << eol;
        return false;
    }

    if (bytesAfterVariableHdr == 0) {
        taskLogger() << logWarnMQTT
                     << "Received MQTT UNSUBSCRIBE message without any Topic Filters." << eol;
        return false;
    }

    payloadStart = (uint8_t *)variableHeaderStart + sizeof(MQTTUnsubscribeVariableHeader);
    uint8_t *payloadPos = payloadStart;
    uint32_t payloadBytesRemaining = bytesAfterVariableHdr;

    // Run through the topic filters making sure the message is properly formed.
    topicFilters = 0;
    do {
        MQTTString *topicFilterStr;
        if (!parseString(topicFilterStr, payloadPos, payloadBytesRemaining)) {
            taskLogger() << logWarnMQTT
                         << "MQTT UNSUBSCRIBE message with payload too small for its Topic Filter"
                         << eol;
            return false;
        }

        if (topicFilterStr->length() == 0) {
            taskLogger() << logWarnMQTT << "MQTT UNSUBSCRIBE message with zero length Topic Filter"
                         << eol;
            return false;
        }

        topicFilters++;
    } while (payloadBytesRemaining);

    topicFiltersPos = payloadStart;
    topicFiltersReturned = 0;

    return true;
}

bool MQTTUnsubscribeMessage::getTopicFilter(MQTTString * &topicFilter) {
    if (topicFiltersReturned == topicFilters) {
        return false;
    }

    grabString(topicFilter, topicFiltersPos);

    topicFiltersReturned++;

    return true;
}

uint16_t MQTTUnsubscribeMessage::packetId() const {
    return variableHeader->packetIdMSB * 256 + variableHeader->packetIdLSB;
}

unsigned MQTTUnsubscribeMessage::numTopicFilters() const {
    return topicFilters;
}
