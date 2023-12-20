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

#include "MQTTString.h"

#include "Logger.h"

#include <etl/string.h>
#include <etl/string_view.h>

#include <stdint.h>
#include <stddef.h>

uint16_t MQTTString::length() const {
    return lengthMSB * 256 + lengthLSB;
}

uint32_t MQTTString::size() const {
    return length() + sizeof(MQTTString);
}

// Returns false if too long to copy. maxLength is max in source and does not include nil.
bool MQTTString::copyTo(char *cString, unsigned maxLength) const {
    const unsigned length = this->length();
    if (length > maxLength) {
        return false;
    }

    unsigned pos;
    for (pos = 0; pos < length; pos++) {
        cString[pos] = characterData[pos];
    }
    cString[pos] = 0;

    return true;
}

// Returns false if too long to copy. maxLength is max in source and does not include nil.
bool MQTTString::copyTo(etl::istring &destString) const {
    const uint16_t length = this->length();
    if (length > destString.max_size()) {
        return false;
    }

    destString.assign(characterData, length);

    return true;
}

Logger & operator << (Logger &logger, const MQTTString &mqttString) {
    const etl::string_view mqttStringView(mqttString.characterData, (size_t)mqttString.length());
    return logger << mqttStringView;
}
