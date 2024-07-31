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

#include "NMEALine.h"
#include "NMEATalker.h"
#include "NMEAUInt8.h"
#include "NMEAMsgType.h"

#include "DataModelUInt8Leaf.h"

#include "Logger.h"
#include "CharacterTools.h"
#include "StringTools.h"
#include "Error.h"

#include "etl/string_view.h"
#include "etl/to_arithmetic.h"

#include <stdint.h>
#include <stddef.h>

bool NMEAUInt8::set(const etl::string_view &valueView, bool optional, uint8_t maxValue) {
    const size_t length = valueView.size();
    if (length == 0) {
        if (!optional) {
            return false;
        }
        valuePresent = false;
        return true;
    }

    etl::to_arithmetic_result<uint8_t> conversionResult = etl::to_arithmetic<uint8_t>(valueView);
    if (!conversionResult.has_value()) {
        valuePresent = false;
        return false;
    }

    if (conversionResult.value() > maxValue) {
        valuePresent = false;
        return false;
    }

    value = conversionResult.value();
    valuePresent = true;
    return true;
}

bool NMEAUInt8::extract(NMEALine &nmeaLine, const NMEATalker &talker, const char *msgTypeName,
                        const char *fieldName, bool optional, uint8_t maxValue) {
    etl::string_view valueView;
    if (!nmeaLine.getWord(valueView)) {
        if (!optional) {
            logger() << logWarnNMEA << talker << " " << msgTypeName << " message missing " << fieldName
                     << " field" << eol;
            return false;
        }

        valuePresent = false;
        return true;
    }

    if (!set(valueView, optional, maxValue)) {
        logger() << logWarnNMEA << talker << " " << msgTypeName << " message with bad " << fieldName
                 << " field '" << valueView << "'" << eol;
        return false;
    }

    return true;
}

bool NMEAUInt8::hasValue() const {
    return valuePresent;
}

NMEAUInt8::operator uint8_t() const {
    if (!valuePresent) {
        fatalError("Attempt to read value from NMEAUInt8 with value not present");
    }

    return value;
}

void NMEAUInt8::publish(DataModelUInt8Leaf &leaf) const {
    if (valuePresent) {
        leaf = value;
    } else {
        leaf.removeValue();
    }
}

void NMEAUInt8::log(Logger &logger) const {
    if (valuePresent) {
        logger << value;
    } else {
        logger << "NA";
    }
}
