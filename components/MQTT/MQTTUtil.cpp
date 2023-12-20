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

#include "MQTTUtil.h"

#include <stdint.h>
#include <sys/socket.h>

bool mqttWriteRemainingLength(int connectionSocket, uint32_t remainingLength) {
    do {
        uint8_t encodedByte;
        encodedByte = remainingLength % 0x80;
        remainingLength = remainingLength / 0x80;
        if (remainingLength) {
            encodedByte |= 0x80;
        }
        if (send(connectionSocket, &encodedByte, sizeof(uint8_t), 0) < 0) {
            return false;
        }
    } while (remainingLength);

    return true;
}

bool mqttWriteUInt16(int connectionSocket, uint16_t value) {
    uint8_t valueBytes[2];
    valueBytes[0] = value >> 8;
    valueBytes[1] = value & 0xff;

    if (send(connectionSocket, valueBytes, 2, 0) < 0) {
        return false;
    }

    return true;
}

bool mqttWriteMQTTString(int connectionSocket, const char *string) {
    const uint16_t length = strlen(string);
    if (!mqttWriteUInt16(connectionSocket, length)) {
        return false;
    }

    if (send(connectionSocket, string, length, 0) < 0) {
        return false;
    }

    return true;
}
