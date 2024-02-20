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

#include "MQTTSubscribeAckMessage.h"

#include "MQTTMessage.h"
#include "MQTTUtil.h"

#include <sys/socket.h>

uint8_t mqttSubscribeResult(bool success, uint8_t maxQoS) {
    if (!success) {
        return MQTT_SUBACK_FAILURE_FLAG;
    } else {
        return maxQoS;
    }
}

bool sendMQTTSubscribeAckMessage(int connectionSocket, uint16_t packetId, uint8_t numberResults,
                                 uint8_t *results) {
    MQTTFixedHeader fixedHeader;
    MQTTSubscribeAckVariableHeader variableHeader;

    fixedHeader.typeAndFlags = MQTT_MSG_SUBACK << MQTT_MSG_TYPE_SHIFT;
    if (send(connectionSocket, &fixedHeader, sizeof(fixedHeader), 0) < 0) {
        return false;
    }
    const uint8_t remainingLength = sizeof(MQTTSubscribeAckVariableHeader) + numberResults;
    if (!mqttWriteRemainingLength(connectionSocket, remainingLength)) {
        return false;
    }

    variableHeader.packetIdMSB = packetId >> 8;
    variableHeader.packetIdLSB = packetId & 0xff;

    if (send(connectionSocket, &variableHeader, sizeof(variableHeader), 0) < 0) {
        return false;
    }

    if (send(connectionSocket, results, numberResults * sizeof(uint8_t), 0) < 0) {
        return false;
    }

    // messagesSent++;

    return true;
}
