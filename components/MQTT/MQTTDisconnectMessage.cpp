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

#include "MQTTDisconnectMessage.h"
#include "MQTTMessage.h"

#include "Logger.h"

MQTTDisconnectMessage::MQTTDisconnectMessage(MQTTMessage const &message) : MQTTMessage(message) {
}

// Returns false if there's a protocol error that requires dropping the connection flat out. We do
// this anyway, despite a correct disconnect message yielding the same result so that we get log
// messages for malformed packets. 
bool MQTTDisconnectMessage::parse() {
    if (fixedHeaderFlags() != 0) {
        taskLogger() << logWarnMQTT
                     << "Received MQTT DISCONNECT message with non-zero Fixed Header Flags." << eol;
        return false;
    }

    if (remainingLength) {
        taskLogger() << logWarnMQTT << "MQTT DISCONNECT packet with " << remainingLength
                     << " extra bytes" << eol;
        return false;
    }

    return true;
}
