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

#ifndef MQTT_STRING_H
#define MQTT_STRING_H

#include <etl/string.h>

#include <stdint.h>

class Logger;

class MQTTString {
    private:
        uint8_t lengthMSB;
        uint8_t lengthLSB;
        char characterData[];

    public:
        uint16_t length() const;
        uint32_t size() const;
        // Returns false if too long to copy. maxLength is max in source and does not include nil.
        bool copyTo(char *cString, unsigned maxLength) const;
        bool copyTo(etl::istring &destString) const;

    friend Logger & operator << (Logger &logger, const MQTTString &mqttString);
};

Logger & operator << (Logger &logger, const MQTTString &mqttString);

#endif
