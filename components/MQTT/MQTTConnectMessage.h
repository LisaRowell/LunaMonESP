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

#ifndef MQTT_CONNECT_MESSAGE_H
#define MQTT_CONNECT_MESSAGE_H

class MQTTString;

#include "MQTTMessage.h"

#include <stdint.h>

struct MQTTConnectVariableHeader {
    uint8_t protocolNameLengthMSB;
    uint8_t protocolNameLengthLSB;
    uint8_t protocolName[4];
    uint8_t level;
    uint8_t flags;
    uint8_t keepAliveMSB;
    uint8_t keepAliveLSB;
};

#define MQTT_PROTOCOL_LEVEL 4

#define MQTT_CONNECT_FLAGS_RESERVED_MASK      0x01
#define MQTT_CONNECT_FLAGS_CLEAN_SESSION_MASK 0x02
#define MQTT_CONNECT_FLAGS_WILL_MASK          0x04
#define MQTT_CONNECT_FLAGS_WILL_QOS_MASK      0x18
#define MQTT_CONNECT_FLAGS_WILL_QOS_SHIFT 3
#define MQTT_CONNECT_FLAGS_WILL_RETAIN_MASK   0x20
#define MQTT_CONNECT_FLAGS_PASSWORD_MASK      0x40
#define MQTT_CONNECT_FLAGS_USER_NAME_MASK     0x80

// These two are up for debate.
#define MQTT_MAX_WILL_MESSAGE_LENGTH 255

class MQTTConnectMessage : public MQTTMessage {
    private:
        MQTTConnectVariableHeader *variableHeader;
        MQTTString *clientIDStr;
        MQTTString *willTopicStr;
        MQTTString *willMessageStr;
        MQTTString *userNameStr;
        MQTTString *passwordStr;

    public:
        MQTTConnectMessage(MQTTMessage const &message);
        bool parse();
        uint8_t sanityCheck();
        bool cleanSession();
        bool hasWill();
        uint8_t willQoS();
        bool willRetain();
        bool hasUserName();
        bool hasPassword();
        uint16_t keepAliveSec();
        const MQTTString *clientID();
};

#endif
