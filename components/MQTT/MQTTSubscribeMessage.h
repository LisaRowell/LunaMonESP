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

#ifndef MQTT_SUBSCRIBE_MESSAGE_H
#define MQTT_SUBSCRIBE_MESSAGE_H

#include "MQTTMessage.h"

#include <stdint.h>

class MQTTString;

struct MQTTSubscribeVariableHeader {
    uint8_t packetIdMSB;
    uint8_t packetIdLSB;
};

class MQTTSubscribeMessage : MQTTMessage {
    private:
        MQTTSubscribeVariableHeader *variableHeader;
        uint8_t *payloadStart;
        unsigned topicFilters;
        unsigned topicFiltersReturned;
        uint8_t *topicFiltersPos;

    public:
        MQTTSubscribeMessage(MQTTMessage const &message);
        bool parse();
        bool getTopicFilter(MQTTString * &topicFilter, uint8_t &maxQoS);
        uint16_t packetId() const;
        unsigned numTopicFilters() const;
};

#endif
