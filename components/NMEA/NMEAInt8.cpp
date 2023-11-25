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

#include "NMEAInt8.h"

#include "Logger.h"
#include "CharacterTools.h"
#include "StringTools.h"

#include <etl/string_view.h>
#include <etl/to_arithmetic.h>

#include <stddef.h>
#include <stdint.h>

bool NMEAInt8::set(const etl::string_view &valueView, bool optional, int8_t minValue,
                   int8_t maxValue) {
    const size_t length = valueView.size();
    if (length == 0) {
        if (!optional) {
            return false;
        }
        valuePresent = false;
        return true;
    }

    etl::to_arithmetic_result<int8_t> conversionResult = etl::to_arithmetic<int8_t>(valueView);
    if (!conversionResult.has_value()) {
        valuePresent = false;
        return false;
    }

    if (conversionResult.value() > maxValue || conversionResult.value() < minValue) {
        valuePresent = false;
        return false;
    }

    value = conversionResult.value();
    valuePresent = true;

    return true;
}

bool NMEAInt8::extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType,
                       const char *fieldName, bool optional, int8_t minValue, int8_t maxValue) {
    etl::string_view valueView;
    if (!nmeaLine.getWord(valueView)) {
        if (!optional) {
            logger() << logWarnNMEA << talker << " " << msgType << " message missing " << fieldName
                     << " field" << eol;
            return false;
        }

        valuePresent = false;
        return true;
    }

    if (!set(valueView, optional, minValue, maxValue)) {
        logger() << logWarnNMEA << talker << " " << msgType << " message with bad " << fieldName
                 << " field '" << valueView << "'" << eol;
        return false;
    }

    return true;
}

#if 0
void NMEAInt8::publish(DataModelInt8Leaf &leaf) const {
    leaf = value;
}
#endif

void NMEAInt8::log(Logger &logger) const {
    logger << value;
}
