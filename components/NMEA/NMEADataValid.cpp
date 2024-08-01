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

#include "NMEADataValid.h"
#include "NMEALine.h"
#include "NMEATalker.h"

#include "DataModelBoolLeaf.h"

#include "Logger.h"

#include "etl/string_view.h"

NMEADataValid::NMEADataValid() : valid(false) {
}

bool NMEADataValid::set(etl::string_view &dataValidView, bool optional) {
    if (dataValidView.size() == 0 && optional) {
        valid = false;
        return true;
    }
    if (dataValidView.size() != 1) {
        return false;
    }

    switch (dataValidView.front()) {
        case 'A':
            valid = true;
            return true;

        case 'V':
            valid = false;
            return true;

        default:
            return false;
    }
}

bool NMEADataValid::extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType,
                            bool optional) {
    etl::string_view dataValidView;
    if (!nmeaLine.getWord(dataValidView)) {
        if (!optional) {
            logger() << logWarnNMEA << talker << " " << msgType << " message missing Data Valid field"
                     << eol;
            return false;
        } else {
            valid = false;
            return true;
        }
    }
    if (!set(dataValidView, optional)) {
        logger() << logWarnNMEA << talker << " " << msgType
                 << " message with bad Data Valid field '" << dataValidView << "'" << eol;
        return false;
    }

    return true;
}

void NMEADataValid::publish(DataModelBoolLeaf &leaf) const {
    leaf = valid;
}

void NMEADataValid::log(Logger &logger) const {
    if (valid) {
        logger << "Valid";
    } else {
        logger << "Invalid";
    }
}
