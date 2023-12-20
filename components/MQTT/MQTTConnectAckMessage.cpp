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

#include "MQTTConnectAckMessage.h"

#include "MQTTMessage.h"
#include "MQTTUtil.h"

#include <stdint.h>
#include <sys/socket.h>

bool sendMQTTConnectAckMessage(int connectionSocket, bool sessionPresent, uint8_t returnCode) {
    MQTTFixedHeader fixedHeader;
    MQTTConnectAckVariableHeader variableHeader;

    fixedHeader.typeAndFlags = MQTT_MSG_CONNACK << MQTT_MSG_TYPE_SHIFT;
    if (send(connectionSocket, &fixedHeader, sizeof(fixedHeader), 0) < 0) {
        return false;
    }
    const uint8_t remainingLength = sizeof(MQTTConnectAckVariableHeader);
    if (!mqttWriteRemainingLength(connectionSocket, remainingLength)) {
        return false;
    }

    variableHeader.flags = 0;
    if (sessionPresent) {
        variableHeader.flags |= MQTT_CONNACK_SESSION_PRESENT_MASK;
    }
    variableHeader.returnCode = returnCode;

    if (send(connectionSocket, &variableHeader, sizeof(variableHeader), 0) < 0) {
        return false;
    }

//    messagesSent++;

    return true;
}
