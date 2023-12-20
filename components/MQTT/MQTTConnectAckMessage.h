/*
 * This file is part of LunaMon (https://github.com/LisaRowell/LunaMon)
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

#ifndef MQTT_CONNECT_ACK_MESSAGE_H
#define MQTT_CONNECT_ACK_MESSAGE_H

#include <stdint.h>

#define MQTT_CONNACK_ACCEPTED                     0x00
#define MQTT_CONNACK_REFUSED_PROTOCOL_VERSION     0x01
#define MQTT_CONNACK_REFUSED_IDENTIFIER_REJECTED  0x02
#define MQTT_CONNACK_REFUSED_SERVER_UNAVAILABLE   0x03
#define MQTT_CONNACK_REFUSED_USERNAME_OR_PASSWORD 0x04
#define MQTT_CONNACK_REFUSED_NOT_AUTHORIZED       0x05

struct MQTTConnectAckVariableHeader {
    uint8_t flags;
    uint8_t returnCode;
};

#define MQTT_CONNACK_SESSION_PRESENT_MASK 0x01

bool sendMQTTConnectAckMessage(int connectionSocket, bool sessionPresent, uint8_t returnCode);

#endif
