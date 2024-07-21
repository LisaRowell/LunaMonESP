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

#ifndef AIS_NAVIGATION_AID_TYPE_H
#define AIS_NAVIGATION_AID_TYPE_H

#include "etl/bit_stream.h"

class Logger;

class AISNavigationAidType {
    public:
        enum Value : uint8_t {
            NAV_AID_TYPE_UNSPECIFIED,
            NAV_AID_TYPE_REFERENCE_POINT,
            NAV_AID_TYPE_RACON,
            NAV_AID_TYPE_FIXED_STRUCTURE_OFF_SHORE,
            NAV_AID_TYPE_SPARE,
            NAV_AID_TYPE_LIGHT_WITHOUT_SECTORS,
            NAV_AID_TYPE_LIGHT_WITH_SECTORS,
            NAV_AID_TYPE_LEADING_LIGHT_FRONT,
            NAV_AID_TYPE_LEADING_LIGHT_REAR,
            NAV_AID_TYPE_BEACON_CARDINAL_N,
            NAV_AID_TYPE_BEACON_CARDINAL_E,
            NAV_AID_TYPE_BEACON_CARDINAL_S,
            NAV_AID_TYPE_BEACON_CARDINAL_W,
            NAV_AID_TYPE_BEACON_PORT_HAND,
            NAV_AID_TYPE_BEACON_STARBOARD_HAND,
            NAV_AID_TYPE_BEACON_PREF_CHAN_PORT_HAND,
            NAV_AID_TYPE_BEACON_PREF_CHAN_STARBOARD_HAND,
            NAV_AID_TYPE_BEACON_ISOLATED_DANGER,
            NAV_AID_TYPE_BEACON_SAFE_WATER,
            NAV_AID_TYPE_BEACON_SPECIAL_MARK,
            NAV_AID_TYPE_CARDINAL_MARK_N,
            NAV_AID_TYPE_CARDINAL_MARK_E,
            NAV_AID_TYPE_CARDINAL_MARK_S,
            NAV_AID_TYPE_CARDINAL_MARK_W,
            NAV_AID_TYPE_PORT_HAND_MARK,
            NAV_AID_TYPE_STARBOARD_HAND_MARK,
            NAV_AID_TYPE_PREF_CHAN_PORT_HAND,
            NAV_AID_TYPE_PREF_CHAN_STARBOARD_HAND,
            NAV_AID_TYPE_ISOLATED_DANGER,
            NAV_AID_TYPE_SAFE_WATER,
            NAV_AID_TYPE_SPECIAL_MARK,
            NAV_AID_TYPE_LIGHT_VESSEL,
            COUNT
        };

    private:
        Value value;

    public:
        AISNavigationAidType();
        AISNavigationAidType(etl::bit_stream_reader &streamReader);
        constexpr operator Value() const { return value; }
        const char *name() const;
        explicit operator bool() const = delete;

        friend Logger & operator << (Logger &logger,
                                     const AISNavigationAidType &navigationAidType);
};

#endif // AIS_NAVIGATION_AID_TYPE_H
