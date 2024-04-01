/*
 * This file is part of LunaMon (https://github.com/LisaRowell/LunaMonESP)
 * Copyright (C) 2024 Lisa Rowell
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


#include "NMEALine.h"
#include "NMEATalker.h"
#include "NMEAUInt32.h"

#include "DataModelUInt32Leaf.h"
#include "Logger.h"
#include "CharacterTools.h"
#include "StringTools.h"
#include "Error.h"

#include "etl/string_view.h"

#include <stdint.h>
#include <stddef.h>

bool NMEAUInt32::set(const etl::string_view &valueView, bool optional, uint32_t maxValue) {
    const size_t length = valueView.size();
    if (length == 0) {
        if (!optional) {
            valuePresent = false;
            return false;
        }
        valuePresent = false;
        return true;
    }

    if (!extractUInt32FromStringView(valueView, 0, length, value, maxValue)) {
        valuePresent = false;
        return false;
    }

    valuePresent = true;
    return true;
}

bool NMEAUInt32::extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType,
                         const char *fieldName, bool optional, uint32_t maxValue) {
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

bool NMEAUInt32::hasValue() const {
    return valuePresent;
}

NMEAUInt32::operator uint32_t() const {
    if (!valuePresent) {
        fatalError("Attempt to read value from NMEAUInt32 with value not present");
    }

    return value;
}

uint32_t NMEAUInt32::getValue() const {
    if (!valuePresent) {
        fatalError("Attempt to read value from NMEAUInt32 with value not present");
    }

    return value;
}

void NMEAUInt32::publish(DataModelUInt32Leaf &leaf) const {
    if (valuePresent) {
        leaf = value;
    } else {
        leaf.removeValue();
    }
}

void NMEAUInt32::log(Logger &logger) const {
    if (valuePresent) {
        logger << value;
    } else {
        logger << "NA";
    }
}
