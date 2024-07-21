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

#ifndef AIS_NAVIGATION_STATUS_H
#define AIS_NAVIGATION_STATUS_H

#include "etl/bit_stream.h"

#include <stddef.h>
#include <stdint.h>

class Logger;

class AISNavigationStatus {
    public:
        enum Value : uint8_t {
            UNDER_WAY_USING_ENGINE,
            AT_ANCHOR,
            NOT_UNDER_COMMAND,
            RESTRICTED_MANOUEUVERABILITY,
            CONSTRAINED_BY_DRAUGHT,
            MOORED,
            AGROUND,
            ENGAGED_IN_FISHING,
            UNDER_WAY_SAILING,
            RESERVED_HSC,
            RESERVED_WIG,
            POWER_DRIVEN_VESSEL_TOWING_ASTERN,
            POWER_DRIVEN_VESSEL_PUSHING_AHEAD,
            RESERVED_13,
            AIS_SART_IS_ACTIVE,
            UNDEFINED
        };

    private:
        Value value;

    public:
        AISNavigationStatus();
        AISNavigationStatus(etl::bit_stream_reader &streamReader);
        constexpr operator Value() const { return value; }
        const char *name() const;
        explicit operator bool() const = delete;

        friend Logger & operator << (Logger &logger, const AISNavigationStatus &navigationStatus);
};

#endif // AIS_NAVIGATION_STATUS_H
