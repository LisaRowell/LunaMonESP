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

#include "NMEATenthsInt16.h"
#include "NMEALineWalker.h"
#include "NMEATalker.h"

#include "DataModelTenthsInt16Leaf.h"

#include "Logger.h"
#include "Error.h"

#include "etl/string_view.h"
#include "etl/to_arithmetic.h"

NMEATenthsInt16::NMEATenthsInt16() : valuePresent(false) {
}

bool NMEATenthsInt16::set(const etl::string_view &valueView, bool optional) {
    int16_t integer;
    uint8_t tenths;

    if (valueView.size() == 0) {
        if (!optional) {
            valuePresent = false;
            return false;
        }
        valuePresent = false;
        return true;
    }

    etl::string_view integerView;
    size_t periodPos = valueView.find('.');
    if (periodPos == valueView.npos) {
        integerView = etl::string_view(valueView);
    } else {
        integerView = etl::string_view(valueView.begin(), periodPos);
    }
    etl::to_arithmetic_result<int16_t> integerResult = etl::to_arithmetic<int16_t>(integerView);
    if (!integerResult.has_value()) {
        valuePresent = false;
        return false;
    }
    integer = integerResult.value();

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
                fatalError("Bad parsing of TenthsInt16");
        }
    } else {
        tenths = 0;
    }

    value = TenthsInt16(integer, tenths);
    valuePresent = true;
    return true;
}

bool NMEATenthsInt16::extract(NMEALineWalker &lineWalker, NMEATalker &talker, const char *msgType,
                              const char *fieldName, bool optional) {
    etl::string_view valueView;
    if (!lineWalker.getWord(valueView) || valueView.length() == 0) {
        if (!optional) {
            logger() << logWarnNMEA << talker << " " << msgType << " message missing " << fieldName
                     << " field" << eol;
            valuePresent = false;
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

bool NMEATenthsInt16::hasValue() const {
    return valuePresent;
}

void NMEATenthsInt16::publish(DataModelTenthsInt16Leaf &leaf) const {
    if (valuePresent) {
        leaf = value;
    } else {
        leaf.removeValue();
    }
}

void NMEATenthsInt16::log(Logger &logger) const {
    if (valuePresent) {
        value.log(logger);
    } else {
        logger << "NA";
    }
}
