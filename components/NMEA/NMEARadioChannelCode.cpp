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

#include "NMEARadioChannelCode.h"
#include "NMEALine.h"
#include "NMEATalker.h"

// #include "DataModel/DataModelStringLeaf.h"

#include "Logger.h"

#include <etl/string_view.h>

bool NMEARadioChannelCode::set(const etl::string_view &radioChannelCodeView) {
    if (radioChannelCodeView.size() != 1) {
        return false;
    }
    switch (radioChannelCodeView.front()) {
        case 'A':
        case '1':
            radioChannelCode = RADIO_CHANNEL_87B;
            return true;
        case 'B':
        case '2':
            radioChannelCode = RADIO_CHANNEL_88B;
            return true;
        default:
            return false;
    }
}

bool NMEARadioChannelCode::extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType) {
    etl::string_view radioChannelCodeView;
    if (!nmeaLine.getWord(radioChannelCodeView)) {
        logger() << logWarnNMEA << talker << " " << msgType
                 << " message missing Radio Channel Code field" << eol;
        return false;
    }
    if (!set(radioChannelCodeView)) {
        logger() << logWarnNMEA << talker << " " << msgType
                 << " message with bad Radio Channel Code field '" << radioChannelCodeView << "'"
                 << eol;
        return false;
    }

    return true;
}

#if 0
void NMEARadioChannelCode::publish(DataModelStringLeaf &leaf) const {
    switch (radioChannelCode) {
        case RADIO_CHANNEL_87B:
            leaf = "87B";
            break;

        case RADIO_CHANNEL_88B:
            leaf = "88B";
    }
}
#endif

void NMEARadioChannelCode::log(Logger &logger) const {
    switch (radioChannelCode) {
        case RADIO_CHANNEL_87B:
            logger << "87B";
            break;

        case RADIO_CHANNEL_88B:
            logger << "88B";
    }
}
