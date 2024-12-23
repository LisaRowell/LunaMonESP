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

#include "NMEATime.h"
#include "NMEALineWalker.h"
#include "NMEATalker.h"

#include "DataModelStringLeaf.h"

#include "Logger.h"
#include "StringTools.h"

#include "etl/string.h"
#include "etl/string_stream.h"
#include "etl/string_view.h"

NMEATime::NMEATime()
    : hasValue(false) {
}

bool NMEATime::set(const etl::string_view &timeView) {
    if (timeView.size() < 6) {
        return false;
    }

    etl::string_view hoursView(timeView.begin(), 2);
    if (!extractUInt8FromStringView(timeView, 0, 2, hours, 23)) {
        return false;
    }

    if (!extractUInt8FromStringView(timeView, 2, 2, minutes, 59)) {
        return false;
    }

    if (!extractUInt8FromStringView(timeView, 4, 2, seconds, 59)) {
        return false;
    }

    if (timeView.size() > 6) {
        if (timeView[6] != '.') {
            return false;
        }

        secondPrecision = timeView.size() - 7;
        if (!extractUInt32FromStringView(timeView, 7, secondPrecision, secondFraction)) {
            return false;
        }
    } else {
        secondFraction = 0;
        secondPrecision = 0;
    }

    return true;
}

bool NMEATime::extract(NMEALineWalker &lineWalker, NMEATalker &talker, const char *msgType,
                       bool optional) {
    etl::string_view timeView;
    if (!lineWalker.getWord(timeView) || timeView.length() == 0) {
        if (!optional) {
            logger() << logWarnNMEA << talker << " " << msgType << " message missing Time field" << eol;
            return false;
        }

        hasValue = false;
        return true;
    }

    if (!set(timeView)) {
        logger() << logWarnNMEA << talker << " " << msgType << " message with bad Time field '"
                 << timeView << "'" << eol;
        hasValue = false;
        return false;
    } else {
        hasValue = true;
    }

    return true;
}

void NMEATime::publish(DataModelStringLeaf &leaf) const {
    if (hasValue) {
        char secondFractionStr[12];
        buildSecondsFactionString(secondFractionStr);

        etl::string<22> timeStr;
        etl::string_stream timeStrStream(timeStr);
        timeStrStream << etl::setfill('0') << etl::setw(2) << hours << etl::setw(1) << ":"
                    << etl::setw(2) << minutes << etl::setw(1) << ":" << etl::setw(2) << seconds
                    << etl::setw(0) << secondFractionStr;

        leaf = timeStr;
    } else {
        leaf.removeValue();
    }
}

void NMEATime::buildSecondsFactionString(char *string) const {
    if (secondPrecision > 0) {
        *string = '.';
        string++;

        char fractionDigits[11];
        sprintf(fractionDigits, "%lu", secondFraction);

        unsigned leadingZeros = secondPrecision - strlen(fractionDigits);
        while (leadingZeros--) {
            *string = '0';
            string++;
        }

        strcpy(string, fractionDigits);
    } else {
        *string = 0;
    }
}

void NMEATime::log(Logger &logger) const {
    if (hasValue) {
        char secondFractionStr[12];
        buildSecondsFactionString(secondFractionStr);

        etl::string<22> timeStr;
        etl::string_stream timeStrStream(timeStr);
        timeStrStream << etl::setfill('0') << etl::setw(2) << hours << etl::setw(1) << ":"
                    << etl::setw(2) << minutes << etl::setw(1) << ":" << etl::setw(2) << seconds
                    << etl::setw(0) << secondFractionStr;

        logger << timeStr;
    } else {
        logger << "NA";
    }
}
