/*
 * This file is part of LunaMon (https://github.com/LisaRowell/LunaMonESP)
 * Copyright (C) 2021-2023 Lisa Rowell
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

#include "MQTTMessage.h"
#include "MQTTString.h"

#include "Error.h"

#include <stdint.h>

MQTTMessage::MQTTMessage()
    : fixedHeader(nullptr), fixedHeaderLength(0), remainingLength(0), variableHeaderStart(nullptr) {
}

MQTTMessage::MQTTMessage(MQTTFixedHeader *fixedHeader, size_t fixedHeaderLength,
                         size_t remainingLength)
    : fixedHeader(fixedHeader), fixedHeaderLength(fixedHeaderLength),
      remainingLength(remainingLength) {
    variableHeaderStart = ((uint8_t *)fixedHeader) + fixedHeaderLength;
}

MQTTMessageType MQTTMessage::messageType() const {
    uint8_t typeValue =
        (MQTTMessageType)(fixedHeader->typeAndFlags & MQTT_MSG_TYPE_MASK) >> MQTT_MSG_TYPE_SHIFT;
    return (MQTTMessageType)typeValue;
}

const char *MQTTMessage::messageTypeStr() const {
    switch (messageType()) {
        case  MQTT_MSG_RESERVED1:
            return "Reserved (0)";
        case MQTT_MSG_CONNECT:
            return "CONNECT";
        case MQTT_MSG_CONNACK:
            return "CONNACK";
        case MQTT_MSG_PUBLISH:
            return "PUBLISH";
        case MQTT_MSG_PUBACK:
            return "PUBACK";
        case MQTT_MSG_PUBREC:
            return "PUBREC";
        case MQTT_MSG_PUBREL:
            return "PUBREL";
        case MQTT_MSG_PUBCOMP:
            return "PUBCOMP";
        case MQTT_MSG_SUBSCRIBE:
            return "SUBSCRIBE";
        case MQTT_MSG_SUBACK:
            return "SUBACK";
        case MQTT_MSG_UNSUBSCRIBE:
            return "UNSUBSCRIBE";
        case MQTT_MSG_UNSUBACK:
            return "UNSUBACK";
        case MQTT_MSG_PINGREQ:
            return "PINGREQ";
        case MQTT_MSG_PINGRESP:
            return "PINGRESP";
        case MQTT_MSG_DISCONNECT:
            return "DISCONNECT";
        case MQTT_MSG_RESERVED2:
            return "RESERVED (15)";
        default:
            fatalError("Wonkie MQTT message type enum");
    }
}

uint8_t MQTTMessage::fixedHeaderFlags() const {
    return (fixedHeader->typeAndFlags & MQTT_MSG_FLAGS_MASK) >> MQTT_MSG_FLAGS_SHIFT;
}

uint32_t MQTTMessage::fixedHeaderSize() const {
    if ((fixedHeader->remainingLength[0] & 0x80) == 0) {
        return sizeof(MQTTFixedHeader) + 1;
    } else if ((fixedHeader->remainingLength[1] & 0x80) == 0) {
        return sizeof(MQTTFixedHeader) + 2;
    } else if ((fixedHeader->remainingLength[2] & 0x80) == 0) {
        return sizeof(MQTTFixedHeader) + 3;
    } else {
        return sizeof(MQTTFixedHeader) + 4;
    }
}

bool MQTTMessage::parseString(MQTTString * &string, uint8_t * &messagePos,
                              uint32_t &bytesRemaining) {
    if (bytesRemaining < sizeof(MQTTString)) {
        return false;
    }

    string = (MQTTString *)messagePos;
    const uint32_t stringSize = string->size();
    if (stringSize > bytesRemaining) {
        return false;
    }

    messagePos += stringSize;
    bytesRemaining -= stringSize;

    return true;
}

void MQTTMessage::grabString(MQTTString * &string, uint8_t * &messagePos) {
    string = (MQTTString *)messagePos;
    const uint32_t stringSize = string->size();

    messagePos += stringSize;
}
