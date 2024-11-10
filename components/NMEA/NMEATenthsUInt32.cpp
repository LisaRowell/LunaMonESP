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

#include "NMEALineWalker.h"
#include "NMEATalker.h"
#include "NMEATenthsUInt32.h"

#include "DataModelTenthsUInt32Leaf.h"

#include "TenthsUInt32.h"

#include "Logger.h"
#include "Error.h"

#include "etl/string_view.h"
#include "etl/to_arithmetic.h"
#include "etl/string.h"
#include "etl/string_stream.h"

#include <stdint.h>

NMEATenthsUInt32::NMEATenthsUInt32() : valuePresent(false) {
}

bool NMEATenthsUInt32::set(const etl::string_view &valueView, bool optional) {
    uint32_t wholeNumber;
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
    etl::to_arithmetic_result<uint32_t> wholeNumberResult
        = etl::to_arithmetic<uint32_t>(wholeNumberView);
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
                fatalError("Bad parsing of TenthsUInt32");
        }
    } else {
        tenths = 0;
    }

    value = TenthsUInt32(wholeNumber, tenths);
    valuePresent = true;
    return true;
}

bool NMEATenthsUInt32::extract(NMEALineWalker &lineWalker, NMEATalker &talker, const char *msgType,
                               const char *fieldName, bool optional) {
    etl::string_view valueView;
    if (!lineWalker.getWord(valueView) || valueView.length() == 0) {
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

bool NMEATenthsUInt32::hasValue() const {
    return valuePresent;
}

void NMEATenthsUInt32::publish(DataModelTenthsUInt32Leaf &leaf) const {
    if (valuePresent) {
        leaf = value;
    } else {
        leaf.removeValue();
    }
}

void NMEATenthsUInt32::log(Logger &logger) const {
    if (valuePresent) {
        value.log(logger);
    } else {
        logger << "NA";
    }
}
