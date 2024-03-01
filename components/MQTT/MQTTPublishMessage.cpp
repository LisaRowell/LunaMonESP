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

#include "MQTTPublishMessage.h"

#include "MQTTMessage.h"
#include "MQTTUtil.h"

#include <stdint.h>
#include <sys/socket.h>

bool sendMQTTPublishMessage(int connectionSocket, const char *topic, const char *value, bool dup,
                            uint8_t qosLevel, bool retain, uint16_t packetId) {
    MQTTFixedHeader fixedHeader;

    fixedHeader.typeAndFlags = MQTT_MSG_PUBLISH << MQTT_MSG_TYPE_SHIFT;
    if (dup) {
        fixedHeader.typeAndFlags |= MQTT_PUBLISH_FLAGS_DUP_MASK;
    }
    fixedHeader.typeAndFlags |= qosLevel << MQTT_PUBLISH_FLAGS_QOS_SHIFT;
    if (retain) {
        fixedHeader.typeAndFlags |= MQTT_PUBLISH_FLAGS_RETAIN_MASK;
    }
    if (send(connectionSocket, &fixedHeader, sizeof(fixedHeader), 0) < 0) {
//        publishMessagesDropped++;
        return false;
    }

    const size_t valueLength = strlen(value);
    uint32_t remainingLength;
    remainingLength = strlen(topic) + 2 + valueLength;
    if (qosLevel > 0) {
        remainingLength += 2;
    }
    if (!mqttWriteRemainingLength(connectionSocket, remainingLength)) {
//        publishMessagesDropped++;
        return false;
    }

    if (!mqttWriteMQTTString(connectionSocket, topic)) {
//        publishMessagesDropped++;
        return false;
    }

    if (qosLevel > 0) {
        if (!mqttWriteUInt16(connectionSocket, packetId)) {
//            publishMessagesDropped++;
            return false;
        }
    }

    if (send(connectionSocket, value, valueLength, 0) < 0) {
//        publishMessagesDropped++;
        return false;
    }

//    messagesSent++;
//    publishMessagesSent++;

    return true;
}
