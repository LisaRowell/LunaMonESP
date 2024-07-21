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

#include "AISNavigationStatus.h"

#include "Logger.h"

#include "etl/bit_stream.h"

#include <stddef.h>
#include <stdint.h>

AISNavigationStatus::AISNavigationStatus() {
    value = UNDEFINED;
}

AISNavigationStatus::AISNavigationStatus(etl::bit_stream_reader &streamReader) {
    const uint8_t navigationStatusCode = etl::read_unchecked<uint8_t>(streamReader, 4);

    value = (enum AISNavigationStatus::Value)navigationStatusCode;
}

const char *AISNavigationStatus::name() const {
    switch (value) {
        case UNDER_WAY_USING_ENGINE:
            return "Under way using engine";
        case AT_ANCHOR:
            return "At anchor";
        case NOT_UNDER_COMMAND:
            return "Not under command";
        case RESTRICTED_MANOUEUVERABILITY:
            return "Restricted manoeuverability";
        case CONSTRAINED_BY_DRAUGHT:
            return "Constrained by her draught";
        case MOORED:
            return "Moored";
        case AGROUND:
            return "Aground";
        case ENGAGED_IN_FISHING:
            return "Engaged in Fishing";
        case UNDER_WAY_SAILING:
            return "Under way sailing";
        case RESERVED_HSC:
            return "Reserved (HSC)";
        case RESERVED_WIG:
            return "Reserved (WIG)";
        case POWER_DRIVEN_VESSEL_TOWING_ASTERN:
            return "Power-driven vessel towing astern";
        case POWER_DRIVEN_VESSEL_PUSHING_AHEAD:
            return "Power-driven vessel pushing ahead";
        case RESERVED_13:
            return "Reserved (13)";
        case AIS_SART_IS_ACTIVE:
            return "AIS-SART is active";
        case UNDEFINED:
        default:
            return "Undefined";
    }
}

Logger & operator << (Logger &logger, const AISNavigationStatus &navigationStatus) {
    logger << navigationStatus.name();

    return logger;
}
