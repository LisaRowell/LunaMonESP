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

#include "NMEANavAidType.h"

#include "Logger.h"

#include "etl/bit_stream.h"

#include <stdint.h>

void NMEANavAidType::parse(etl::bit_stream_reader &streamReader) {
        type = (enum NavAidTypeEnum)etl::read_unchecked<uint8_t>(streamReader, 5);
}

const char *NMEANavAidType::toString() const {
    switch (type) {
        case NAV_AID_TYPE_UNSPECIFIED:
            return "Unspecified";
        case NAV_AID_TYPE_REFERENCE_POINT:
            return "Reference point";
        case NAV_AID_TYPE_RACON:
            return "RACON";
        case NAV_AID_TYPE_FIXED_STRUCTURE_OFF_SHORE:
            return "Fixed structure off shore";
        case NAV_AID_TYPE_SPARE:
            return "Spare";
        case NAV_AID_TYPE_LIGHT_WITHOUT_SECTORS:
            return "Light without sectors";
        case NAV_AID_TYPE_LIGHT_WITH_SECTORS:
            return "Light with sectors";
        case NAV_AID_TYPE_LEADING_LIGHT_FRONT:
            return "Leading Light Front";
        case NAV_AID_TYPE_LEADING_LIGHT_REAR:
            return "Leading Light Rear";
        case NAV_AID_TYPE_BEACON_CARDINAL_N:
            return "Beacon, Cardinal N";
        case NAV_AID_TYPE_BEACON_CARDINAL_E:
            return "Beacon, Cardinal E";
        case NAV_AID_TYPE_BEACON_CARDINAL_S:
            return "Beacon, Cardinal S";
        case NAV_AID_TYPE_BEACON_CARDINAL_W:
            return "Beacon, Cardinal W";
        case NAV_AID_TYPE_BEACON_PORT_HAND:
            return "Beacon, Port hand";
        case NAV_AID_TYPE_BEACON_STARBOARD_HAND:
            return "Beacon, Starboard hand";
        case NAV_AID_TYPE_BEACON_PREF_CHAN_PORT_HAND:
            return "Beacon, Preferred Channel port hand";
        case NAV_AID_TYPE_BEACON_PREF_CHAN_STARBOARD_HAND:
            return "Beacon, Preferred Channel starboard hand";
        case NAV_AID_TYPE_BEACON_ISOLATED_DANGER:
            return "Beacon, Isolated danger";
        case NAV_AID_TYPE_BEACON_SAFE_WATER:
            return "Beacon, Safe water";
        case NAV_AID_TYPE_BEACON_SPECIAL_MARK:
            return "Beacon, Special mark";
        case NAV_AID_TYPE_CARDINAL_MARK_N:
            return "Cardinal Mark N";
        case NAV_AID_TYPE_CARDINAL_MARK_E:
            return "Cardinal Mark E";
        case NAV_AID_TYPE_CARDINAL_MARK_S:
            return "Cardinal Mark S";
        case NAV_AID_TYPE_CARDINAL_MARK_W:
            return "Cardinal Mark W";
        case NAV_AID_TYPE_PORT_HAND_MARK:
            return "Port hand Mark";
        case NAV_AID_TYPE_STARBOARD_HAND_MARK:
            return "Starboard hand Mark";
        case NAV_AID_TYPE_PREF_CHAN_PORT_HAND:
            return "Preferred Channel Port hand";
        case NAV_AID_TYPE_PREF_CHAN_STARBOARD_HAND:
            return "Preferred Channel Starboard hand";
        case NAV_AID_TYPE_ISOLATED_DANGER:
            return "Isolated danger";
        case NAV_AID_TYPE_SAFE_WATER:
            return "Safe Water";
        case NAV_AID_TYPE_SPECIAL_MARK:
            return "Special Mark";
        case NAV_AID_TYPE_LIGHT_VESSEL:
            return "Light Vessel";
        default:
            return "Unknown";
    }
}

void NMEANavAidType::log(Logger &logger) const {
    logger << toString();
}
