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
#include "NMEATenthsUInt16.h"

#include "DataModelTenthsUInt16Leaf.h"

#include "TenthsUInt16.h"

#include "Logger.h"
#include "Error.h"

#include "etl/string_view.h"
#include "etl/to_arithmetic.h"
#include "etl/string.h"
#include "etl/string_stream.h"

#include <stdint.h>

NMEATenthsUInt16::NMEATenthsUInt16() : valuePresent(false) {
}

bool NMEATenthsUInt16::set(const etl::string_view &valueView, bool optional) {
    uint16_t wholeNumber;
    uint8_t tenths;

    if (valueView.size() == 0) {
        if (!optional) {
            valuePresent = false;
            return false;
        }
        valuePresent = false;
        return true;
    }

    etl::string_view wholeNumberView;
    size_t periodPos = valueView.find('.');
    if (periodPos == valueView.npos) {
        wholeNumberView = etl::string_view(valueView);
    } else {
        wholeNumberView = etl::string_view(valueView.begin(), periodPos);
    }
    etl::to_arithmetic_result<uint16_t> wholeNumberResult
        = etl::to_arithmetic<uint16_t>(wholeNumberView);
    if (!wholeNumberResult.has_value()) {
        valuePresent = false;
        return false;
    }
    wholeNumber = wholeNumberResult.value();

    if (periodPos != valueView.npos && valueView.length() > periodPos + 1) {
        etl::string_view decimalView(valueView.begin() + periodPos, valueView.end());
        decimalView.remove_prefix(1);
        if (decimalView.length() > 2) {
            decimalView.remove_suffix(decimalView.length() - 2);
        }
        etl::to_arithmetic_result<uint8_t> decimalResult = etl::to_arithmetic<uint8_t>(decimalView);
        if (!decimalResult.has_value()) {
            valuePresent = false;
            return false;
        }
        switch (decimalView.length()) {
            case 1:
                tenths = decimalResult.value();
                break;
            case 2:
                tenths = (decimalResult.value() + 5) / 10;
                break;
            default:
                fatalError("Bad parsing of TenthsUint16");
        }
    } else {
        tenths = 0;
    }

    value = TenthsUInt16(wholeNumber, tenths);
    valuePresent = true;
    return true;
}

bool NMEATenthsUInt16::extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType,
                               const char *fieldName, bool optional) {
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

    if (!set(valueView, optional)) {
        logger() << logWarnNMEA << talker << " " << msgType << " message with bad " << fieldName
                 << " field '" << valueView << "'" << eol;
        return false;
    }

    return true;
}

bool NMEATenthsUInt16::hasValue() const {
    return valuePresent;
}

void NMEATenthsUInt16::publish(DataModelTenthsUInt16Leaf &leaf) const {
    if (valuePresent) {
        leaf = value;
    } else {
        leaf.removeValue();
    }
}

void NMEATenthsUInt16::log(Logger &logger) const {
    if (valuePresent) {
        value.log(logger);
    } else {
        logger << "NA";
    }
}
