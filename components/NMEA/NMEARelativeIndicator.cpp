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

#include "NMEARelativeIndicator.h"
#include "NMEALine.h"
#include "NMEATalker.h"

#include "Logger.h"

#include "etl/string_view.h"

NMEARelativeIndicator::NMEARelativeIndicator() : relative(true) {
}

bool NMEARelativeIndicator::set(etl::string_view &relativeIndicatorView) {
    if (relativeIndicatorView.size() != 1) {
        return false;
    }

    switch (relativeIndicatorView.front()) {
        case 'R':
            relative = true;
            return true;

        case 'T':
            relative = false;
            return true;

        default:
            return false;
    }
}

bool NMEARelativeIndicator::extract(NMEALine &nmeaLine, const NMEATalker &talker,
                                    const char *msgType) {
    etl::string_view relativeIndicatorView;
    if (!nmeaLine.getWord(relativeIndicatorView)) {
        logger() << logWarnNMEA << talker << " " << msgType
                 << " message missing Relative Indicator field" << eol;
        return false;
    }
    if (!set(relativeIndicatorView)) {
        logger() << logWarnNMEA << talker << " " << msgType
                 << " message with bad Relative Indicator field '" << relativeIndicatorView << "'"
                 << eol;
        return false;
    }

    return true;
}

bool NMEARelativeIndicator::isRelative() const {
    return relative;
}

void NMEARelativeIndicator::log(Logger &logger) const {
    if (relative) {
        logger << "Relative";
    } else {
        logger << "True";
    }
}
