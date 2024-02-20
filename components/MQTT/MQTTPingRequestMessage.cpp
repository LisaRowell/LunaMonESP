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

#include "MQTTPingRequestMessage.h"

#include "Logger.h"

MQTTPingRequestMessage::MQTTPingRequestMessage(MQTTMessage const &message) : MQTTMessage(message) {
}

bool MQTTPingRequestMessage::parse() {
    if (fixedHeaderFlags() != 0x0) {
        taskLogger() << logWarnMQTT
                     << "Received MQTT PINGREQ message with invalid Fixed Header Flags" << eol;
        return false;
    }

    if (remainingLength > 0) {
        taskLogger() << logWarnMQTT
                     << "Received MQTT PINGREQ message with extra data after Fixed Header" << eol;
        return false;
    }

    return true;
}
