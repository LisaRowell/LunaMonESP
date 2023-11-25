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

#include "NMEAUInt16.h"

#include "Logger.h"
#include "CharacterTools.h"
#include "StringTools.h"
#include "Error.h"

#include <etl/string_view.h>

#include <stdint.h>
#include <stddef.h>

bool NMEAUInt16::set(const etl::string_view &valueView, bool optional, uint16_t maxValue) {
    const size_t length = valueView.size();
    if (length == 0) {
        if (!optional) {
            valuePresent = false;
            return false;
        }
        valuePresent = false;
        return true;
    }

    if (!extractUInt16FromStringView(valueView, 0, length, value, maxValue)) {
        valuePresent = false;
        return false;
    }

    valuePresent = true;
    return true;
}

bool NMEAUInt16::extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType,
                         const char *fieldName, bool optional, uint16_t maxValue) {
    etl::string_view valueView;
    if (!nmeaLine.getWord(valueView)) {
        if (!optional) {
            logger() << logWarnNMEA << talker << " " << msgType << " message missing " << fieldName
                     << " field" << eol;
            valuePresent = false;
            return false;
        }

        valuePresent = false;
        return true;
    }

    if (!set(valueView, optional, maxValue)) {
        logger() << logWarnNMEA << talker << " " << msgType << " message with bad " << fieldName
                 << " field '" << valueView << "'" << eol;
        return false;
    }

    return true;
}

bool NMEAUInt16::hasValue() const {
    return valuePresent;
}

uint16_t NMEAUInt16::getValue() const {
    if (!valuePresent) {
        fatalError("Attempt to read value from NMEAUInt8 with value not present");
    }

    return value;
}

#if 0
void NMEAUInt16::publish(DataModelUInt16Leaf &leaf) const {
    if (valuePresent) {
        leaf = value;
    } else {
        leaf.removeValue();
    }
}
#endif

void NMEAUInt16::log(Logger &logger) const {
    if (valuePresent) {
        logger << value;
    } else {
        logger << "NA";
    }
}
