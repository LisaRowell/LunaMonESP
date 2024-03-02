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

#include "NMEADate.h"
#include "NMEALine.h"
#include "NMEATalker.h"

#include "DataModelStringLeaf.h"

#include "Logger.h"

#include "etl/string.h"
#include "etl/string_view.h"
#include "etl/string_stream.h"
#include "etl/to_arithmetic.h"

#include <stddef.h>

bool NMEADate::set(const etl::string_view &dateView) {
    const size_t length = dateView.size();
    if (length == 0) {
        hasValue = false;
        return true;
    }

    if (length != 6) {
        hasValue = false;
        return false;
    }

    etl::string_view dayView(dateView.data(), dateView.data() + 2);
    etl::to_arithmetic_result<uint8_t> dayResult = etl::to_arithmetic<uint8_t>(dayView);
    if (!dayResult.has_value()) {
        hasValue = false;
        return false;
    }
    if (dayResult.value() > 31) {
        hasValue = false;
        return false;
    }
    day = dayResult.value();

    etl::string_view monthView(dateView.data() + 2, dateView.data() + 4);
    etl::to_arithmetic_result<uint8_t> monthResult = etl::to_arithmetic<uint8_t>(monthView);
    if (!monthResult.has_value()) {
        hasValue = false;
        return false;
    }
    if (monthResult.value() > 12) {
        hasValue = false;
        return false;
    }
    month = monthResult.value();

    // NMEA 0183 has a problem with date fields having a two digit year. How this made it into a
    // specification is beyond my words. Make a year by using a century in the configuration.
    etl::string_view yearView(dateView.data() + 4, dateView.data() + 6);
    etl::to_arithmetic_result<uint8_t> yearResult = etl::to_arithmetic<uint8_t>(yearView);
    if (!yearResult.has_value()) {
        hasValue = false;
        return false;
    }
    year = yearResult.value() + 2000;
    hasValue = true;

    return true;
}

bool NMEADate::extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType) {
    etl::string_view dateView;
    if (!nmeaLine.getWord(dateView)) {
        logger() << logWarnNMEA << talker << " " << msgType << " message missing Date field" << eol;
        return false;
    }

    if (!set(dateView)) {
        logger() << logWarnNMEA << talker << " " << msgType << " message with bad Date field '"
                 << dateView << "'" << eol;
        return false;
    }

    return true;
}

void NMEADate::publish(DataModelStringLeaf &leaf) const {
    if (hasValue) {
        // We use the US format of mm/dd/yyyyy.
        etl::string<dateLength> dateStr;
        etl::string_stream dateStrStream(dateStr);
        dateStrStream << etl::setfill('0') << etl::setw(2) << month << etl::setw(1) << "/"
                      << etl::setw(2) << day << etl::setw(1) << "/" << etl::setw(4) << year;        
        leaf = dateStr;
    } else {
        leaf.removeValue();
    }
}

void NMEADate::log(Logger &logger) const {
    if (hasValue) {
        logger << month / 10 << month % 10 << "/" << day / 10 << day % 10 << "/" << year;
    } else {
        logger << "Unknown";
    }
}
