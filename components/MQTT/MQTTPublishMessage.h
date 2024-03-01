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

#ifndef MQTT_PUBLISH_MESSAGE_H
#define MQTT_PUBLISH_MESSAGE_H

#include <stdint.h>

#define MQTT_PUBLISH_FLAGS_DUP_MASK    0x08
#define MQTT_PUBLISH_FLAGS_QOS_MASK    0x06
#define MQTT_PUBLISH_FLAGS_QOS_SHIFT 1
#define MQTT_PUBLISH_FLAGS_RETAIN_MASK 0x01

bool sendMQTTPublishMessage(int connectionSocket, const char *topic, const char *value, bool dup,
                            uint8_t qosLevel, bool retain, uint16_t packetId);

#endif // MQTT_PUBLISH_MESSAGE_H
