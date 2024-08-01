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

#include "NMEAHeadingOffset.h"
#include "NMEALine.h"
#include "NMEATalker.h"

#include "DataModelTenthsInt16Leaf.h"

#include "TenthsInt16.h"

#include "CharacterTools.h"
#include "Logger.h"
#include "Error.h"

#include "etl/string_view.h"
#include "etl/to_arithmetic.h"

NMEAHeadingOffset::NMEAHeadingOffset() : hasValue(false) {
}

bool NMEAHeadingOffset::set(const etl::string_view &directionView,
                            const etl::string_view &eastOrWestView) {
    if (directionView.size() == 0 || eastOrWestView.size() == 0) {
        hasValue = false;
        return true;
    }

    etl::string_view integerView;
    size_t periodPos = directionView.find('.');
    if (periodPos == directionView.npos) {
        integerView = etl::string_view(directionView);
    } else {
        integerView = etl::string_view(directionView.begin(), periodPos);
    }
    etl::to_arithmetic_result<uint8_t> integerResult = etl::to_arithmetic<uint8_t>(integerView);
    if (!integerResult.has_value()) {
        hasValue = false;
        return false;
    }
    uint8_t directionAbsoluteValue = integerResult.value();

    if (periodPos != directionView.npos && directionView.length() > periodPos + 1) {
        etl::string_view decimalView(directionView.begin() + periodPos, directionView.end());
        decimalView.remove_prefix(1);
        if (decimalView.length() > 2) {
            decimalView.remove_suffix(decimalView.length() - 2);
        }
        etl::to_arithmetic_result<uint8_t> decimalResult = etl::to_arithmetic<uint8_t>(decimalView);
        if (!decimalResult.has_value()) {
            hasValue = false;
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

    if (eastOrWestView.size() != 1) {
        hasValue = false;
        return false;
    }
    switch (eastOrWestView.front()) {
        case 'W':
            direction = directionAbsoluteValue;
            break;
        case 'E':
            direction = -directionAbsoluteValue;
            break;
        default:
            hasValue = false;
            return false;
    }

    hasValue = true;
    return true;
}

bool NMEAHeadingOffset::extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType,
                                const char *fieldName) {
    etl::string_view directionView;
    if (!nmeaLine.getWord(directionView)) {
        logger() << logWarnNMEA << talker << " " << msgType
                 << " message missing " << fieldName << " direction field" << eol;
        hasValue = false;
        return false;
    }
    etl::string_view eastOrWestView;
    if (!nmeaLine.getWord(eastOrWestView)) {
        logger() << logWarnNMEA << talker << " " << msgType
                 << " message missing " << fieldName << " E/W field" << eol;
        hasValue = false;
        return false;
    }

    if (!set(directionView, eastOrWestView)) {
        logger() << logWarnNMEA << talker << " " << msgType
                 << " message with bad " << fieldName << " field '" << directionView << ","
                 << eastOrWestView << "'" << eol;
        return false;
    }

    return true;
}

void NMEAHeadingOffset::publish(DataModelTenthsInt16Leaf &leaf) const {
    if (hasValue) {
        leaf = TenthsInt16(direction, tenths);
    } else {
        leaf.removeValue();
    }
}

void NMEAHeadingOffset::log(Logger &logger) const {
    if (hasValue) {
        logger << direction << "." << tenths << "\xC2\xB0";
    } else {
        logger << "Unknown";
    }
}
