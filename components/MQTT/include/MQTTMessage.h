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

#ifndef MQTT_MESSAGE_H
#define MQTT_MESSAGE_H

class MQTTString;

#include <stdint.h>
#include <stddef.h>

struct MQTTFixedHeader {
    uint8_t typeAndFlags;
    uint8_t remainingLength[];
};

#define MQTT_MSG_TYPE_MASK 0xf0
#define MQTT_MSG_TYPE_SHIFT 4
#define MQTT_MSG_FLAGS_MASK 0x0f
#define MQTT_MSG_FLAGS_SHIFT 0

enum MQTTMessageType : uint8_t {
    MQTT_MSG_RESERVED1 = 0,
    MQTT_MSG_CONNECT = 1,
    MQTT_MSG_CONNACK = 2,
    MQTT_MSG_PUBLISH = 3,
    MQTT_MSG_PUBACK = 4,
    MQTT_MSG_PUBREC = 5,
    MQTT_MSG_PUBREL = 6,
    MQTT_MSG_PUBCOMP = 7,
    MQTT_MSG_SUBSCRIBE = 8,
    MQTT_MSG_SUBACK = 9,
    MQTT_MSG_UNSUBSCRIBE = 10,
    MQTT_MSG_UNSUBACK = 11,
    MQTT_MSG_PINGREQ = 12,
    MQTT_MSG_PINGRESP = 13,
    MQTT_MSG_DISCONNECT = 14,
    MQTT_MSG_RESERVED2 = 15
};

class MQTTMessage {
    protected:
        MQTTFixedHeader *fixedHeader;
        size_t fixedHeaderLength;
        size_t remainingLength;
        void *variableHeaderStart;

        uint8_t fixedHeaderFlags() const;
        uint32_t fixedHeaderSize() const;
        bool parseString(MQTTString * &string, uint8_t * &messagePos, uint32_t &bytesRemaining);
        void grabString(MQTTString * &string, uint8_t *&messagePos);

    public:
        MQTTMessage();
        MQTTMessage(MQTTFixedHeader *fixedHeader, size_t fixedHeaderLength, size_t remainingLength);
        enum MQTTMessageType messageType() const;
        const char *messageTypeStr() const;
};

#endif
