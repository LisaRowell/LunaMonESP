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

#include "NMEACoordinate.h"

// #include "DataModel/DataModel.h"
// #include "DataModel/DataModelStringLeaf.h"

#include "CharacterTools.h"
#include "StringTools.h"
#include "Logger.h"

#include <etl/string_view.h>
#include <etl/to_arithmetic.h>
#include <etl/string.h>
#include <etl/string_stream.h>

#include <stdint.h>

bool NMEACoordinate::setDegrees(const etl::string_view &degreesView, uint8_t maxDegrees) {
    etl::to_arithmetic_result<uint8_t> conversionResult = etl::to_arithmetic<uint8_t>(degreesView);
    if (!conversionResult.has_value()) {
        return false;
    }

    if (conversionResult.value() > maxDegrees) {
        return false;
    }

    degrees = conversionResult.value();
    return true;
}

bool NMEACoordinate::setMinutes(const etl::string_view &minutesView) {
    etl::string_view wholeMinutesView(minutesView.begin(), 2);
    etl::to_arithmetic_result<uint8_t> wholeResult = etl::to_arithmetic<uint8_t>(wholeMinutesView);
    if (!wholeResult.has_value()) {
        return false;
    }
    uint8_t wholeMinutes = wholeResult.value();
    if (wholeMinutes > 59) {
        return false;
    }

    if (minutesView.size() > 2) {
        if (minutesView[2] != '.') {
            return false;
        }

        if (minutesView.size() > 3) {
            etl::string_view decimalView(minutesView.begin() + 2, minutesView.end());
            etl::to_arithmetic_result<float> decimalResult = etl::to_arithmetic<float>(decimalView);
            if (!decimalResult.has_value()) {
                return false;
            }
            minutes = (float)wholeMinutes + decimalResult.value();
        } else {
            minutes = (float)wholeMinutes;
        }
    } else {
        minutes = (float)wholeMinutes;
    }

    return true;
}

// This prints the coordinate as unsigned and the caller is responsible for appending N/S or E/W.
void NMEACoordinate::log(Logger &logger) const {
    etl::string<20> coordinateStr;
    etl::string_stream coordinateStream(coordinateStr);

    coordinateStream << degrees << "\xC2\xB0" << etl::setprecision(5) << minutes << "'";

    logger << coordinateStr;
}

#if 0
void NMEACoordinate::publish(DataModelStringLeaf &leaf, const char *suffix) const {
    etl::string<coordinateLength> coordinateStr;
    etl::string_stream coordinateStream(coordinateStr);

    coordinateStream << degrees << "\xC2\xB0 " << etl::setprecision(4) << minutes << "' " << suffix;

    leaf = coordinateStr;
}
#endif
