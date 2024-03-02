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
#include "NMEAHundredthsUInt8.h"

#include "DataModelHundredthsUInt8Leaf.h"

#include "HundredthsUInt8.h"

#include "Logger.h"
#include "Error.h"

#include "etl/string_view.h"
#include "etl/to_arithmetic.h"
#include "etl/string.h"
#include "etl/string_stream.h"

#include <stdint.h>

bool NMEAHundredthsUInt8::set(const etl::string_view &valueView) {
    uint8_t wholeNumber;
    uint8_t hundredths;

    if (valueView.size() == 0) {
        return false;
    }

    etl::string_view wholeNumberView;
    size_t periodPos = valueView.find('.');
    if (periodPos == valueView.npos) {
        wholeNumberView = etl::string_view(valueView);
    } else {
        wholeNumberView = etl::string_view(valueView.begin(), periodPos);
    }
    etl::to_arithmetic_result<uint8_t> wholeNumberResult
        = etl::to_arithmetic<uint8_t>(wholeNumberView);
    if (!wholeNumberResult.has_value()) {
        return false;
    }
    wholeNumber = wholeNumberResult.value();

    if (periodPos != valueView.npos && valueView.length() > periodPos + 1) {
        etl::string_view decimalView(valueView.begin() + periodPos, valueView.end());
        decimalView.remove_prefix(1);
        if (decimalView.length() > 3) {
            decimalView.remove_suffix(decimalView.length() - 3);
        }
        etl::to_arithmetic_result<uint16_t> decimalResult
            = etl::to_arithmetic<uint16_t>(decimalView);
        if (!decimalResult.has_value()) {
            return false;
        }
        switch (decimalView.length()) {
            case 1:
                hundredths = decimalResult.value() * 10;
                break;
            case 2:
                hundredths = decimalResult.value();
                break;
            case 3:
                hundredths = (decimalResult.value() + 5) / 10;
                break;
            default:
                fatalError("Bad parsing of HundredthsUint8");
        }
    } else {
        hundredths = 0;
    }

    value = HundredthsUInt8(wholeNumber, hundredths);
    return true;
}

bool NMEAHundredthsUInt8::extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType,
                                  const char *fieldName) {
    etl::string_view valueView;
    if (!nmeaLine.getWord(valueView)) {
        logger() << logWarnNMEA << talker << " " << msgType << " message missing " << fieldName
                 << " field" << eol;
        return false;
    }

    if (!set(valueView)) {
        logger() << logWarnNMEA << talker << " " << msgType << " message with bad " << fieldName
                 << " field '" << valueView << "'" << eol;
        return false;
    }

    return true;
}

void NMEAHundredthsUInt8::publish(DataModelHundredthsUInt8Leaf &leaf) const {
    leaf = value;
}

void NMEAHundredthsUInt8::log(Logger &logger) const {
    value.log(logger);
}
