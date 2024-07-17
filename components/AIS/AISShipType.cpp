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

#include "AISShipType.h"

#include "Logger.h"

#include "etl/bit_stream.h"

#include <stddef.h>
#include <stdint.h>

AISShipType::AISShipType() {
    value = NOT_AVAILABLE;
}

AISShipType::AISShipType(etl::bit_stream_reader &streamReader) {
    const uint8_t shipTypeCode = etl::read_unchecked<uint8_t>(streamReader, 8);

    if (shipTypeCode < COUNT) {
        value = (enum AISShipType::Value)shipTypeCode;
    } else {
        value = NOT_AVAILABLE;
    }
}

const char *AISShipType::name() const {
    switch (value) {
        case NOT_AVAILABLE:
            return "Not available";
        case RESERVED_1:
            return "Reserved (1)";
        case RESERVED_2:
            return "Reserved (2)";
        case RESERVED_3:
            return "Reserved (3)";
        case RESERVED_4:
            return "Reserved (4)";
        case RESERVED_5:
            return "Reserved (5)";
        case RESERVED_6:
            return "Reserved (6)";
        case RESERVED_7:
            return "Reserved (7)";
        case RESERVED_8:
            return "Reserved (8)";
        case RESERVED_9:
            return "Reserved (9)";
        case RESERVED_10:
            return "Reserved (10)";
        case RESERVED_11:
            return "Reserved (11)";
        case RESERVED_12:
            return "Reserved (12)";
        case RESERVED_13:
            return "Reserved (13)";
        case RESERVED_14:
            return "Reserved (14)";
        case RESERVED_15:
            return "Reserved (15)";
        case RESERVED_16:
            return "Reserved (16)";
        case RESERVED_17:
            return "Reserved (17)";
        case RESERVED_18:
            return "Reserved (18)";
        case RESERVED_19:
            return "Reserved (19)";
        case WING_IN_GROUND:
            return "Wing in ground (WIG)";
        case WING_IN_GROUND_HAZARDOUS_A:
            return "Wing in ground (WIG), Hazardous A";
        case WING_IN_GROUND_HAZARDOUS_B:
            return "Wing in ground (WIG), Hazardous B";
        case WING_IN_GROUND_HAZARDOUS_C:
            return "Wing in ground (WIG), Hazardous C";
        case WING_IN_GROUND_HAZARDOUS_D:
            return "Wing in ground (WIG), Hazardous D";
        case WING_IN_GROUND_RESERVED_25:
            return "Wing in ground (WIG), Reserved (25)";
        case WING_IN_GROUND_RESERVED_26:
            return "Wing in ground (WIG), Reserved (26)";
        case WING_IN_GROUND_RESERVED_27:
            return "Wing in ground (WIG), Reserved (27)";
        case WING_IN_GROUND_RESERVED_28:
            return "Wing in ground (WIG), Reserved (28)";
        case WING_IN_GROUND_RESERVED_29:
            return "Wing in ground (WIG), Reserved (29)";
        case FISHING:
            return "Fishing";
        case TOWING:
            return "Towing";
        case TOWING_LARGE_VESSEL:
            return "Towing, large vessel";
        case DREDGING_OR_UNDERWATER_OPS:
            return "Dredging or underwater ops";
        case DIVING_OPS:
            return "Diving ops";
        case MILITARY_OPS:
            return "Military ops";
        case SAILING:
            return "Sailing";
        case PLEASURE_CRAFT:
            return "Pleasure Craft";
        case RESERVED_38:
            return "Reserved (38)";
        case RESERVED_39:
            return "Reserved (39)";
        case HIGH_SPEED_CRAFT:
            return "High speed craft (HSC)";
        case HIGH_SPEED_CRAFT_HAZARDOUS_A:
            return "High speed craft (HSC), Hazardous A";
        case HIGH_SPEED_CRAFT_HAZARDOUS_B:
            return "High speed craft (HSC), Hazardous B";
        case HIGH_SPEED_CRAFT_HAZARDOUS_C:
            return "High speed craft (HSC), Hazardous C";
        case HIGH_SPEED_CRAFT_HAZARDOUS_D:
            return "High speed craft (HSC), Hazardous D";
        case HIGH_SPEED_CRAFT_RESERVED_45:
            return "High speed craft (HSC), Reserved (45)";
        case HIGH_SPEED_CRAFT_RESERVED_46:
            return "High speed craft (HSC), Reserved (46)";
        case HIGH_SPEED_CRAFT_RESERVED_47:
            return "High speed craft (HSC), Reserved (47)";
        case HIGH_SPEED_CRAFT_RESERVED_48:
            return "High speed craft (HSC), Reserved (48)";
        case HIGH_SPEED_CRAFT_NO_ADDITIONAL_INFO:
            return "High speed craft (HSC), No additional info";
        case PILOT_VESSEL:
            return "Pilot Vessel";
        case SEARCH_AND_RESCUE_VESSEL:
            return "Search and Rescue vessel";
        case TUG:
            return "Tug";
        case PORT_TENDER:
            return "Port Tender";
        case ANTI_POLLUTION_EQUIPMENT:
            return "Anti-pollution equipment";
        case LAW_ENFORCEMENT:
            return "Law Enforcement";
        case SPARE_LOCAL_VESSEL_56:
            return "Spare - Local Vessel (56)";
        case SPARE_LOCAL_VESSEL_57:
            return "Spare - Local Vessel (57)";
        case MEDICAL_TRANSPORT:
            return "Medical Transport";
        case NONCOMBATANT_SHIP:
            return "Noncombatant ship";
        case PASSENGER:
            return "Passenger";
        case PASSENGER_HAZARDOUS_A:
            return "Passenger, Hazardous A";
        case PASSENGER_HAZARDOUS_B:
            return "Passenger, Hazardous B";
        case PASSENGER_HAZARDOUS_C:
            return "Passenger, Hazardous C";
        case PASSENGER_HAZARDOUS_D:
            return "Passenger, Hazardous D";
        case PASSENGER_RESERVED_65:
            return "Passenger, Reserved (65)";
        case PASSENGER_RESERVED_66:
            return "Passenger, Reserved (66)";
        case PASSENGER_RESERVED_67:
            return "Passenger, Reserved (67)";
        case PASSENGER_RESERVED_68:
            return "Passenger, Reserved (68)";
        case PASSENGER_NO_ADDITIONAL_INFO:
            return "Passenger, No additional info";
        case CARGO:
            return "Cargo";
        case CARGO_HAZARDOUS_A:
            return "Cargo, Hazardous A";
        case CARGO_HAZARDOUS_B:
            return "Cargo, Hazardous B";
        case CARGO_HAZARDOUS_C:
            return "Cargo, Hazardous C";
        case CARGO_HAZARDOUS_D:
            return "Cargo, Hazardous D";
        case CARGO_RESERVED_75:
            return "Cargo, Reserved (75)";
        case CARGO_RESERVED_76:
            return "Cargo, Reserved (76)";
        case CARGO_RESERVED_77:
            return "Cargo, Reserved (77)";
        case CARGO_RESERVED_78:
            return "Cargo, Reserved (78)";
        case CARGO_NO_ADDITIONAL_INFO:
            return "Cargo, No additional info";
        case TANKER:
            return "Tanker";
        case TANKER_HAZARDOUS_A:
            return "Tanker, Hazardous A";
        case TANKER_HAZARDOUS_B:
            return "Tanker, Hazardous B";
        case TANKER_HAZARDOUS_C:
            return "Tanker, Hazardous C";
        case TANKER_HAZARDOUS_D:
            return "Tanker, Hazardous D";
        case TANKER_RESERVED_85:
            return "Tanker, Reserved (85)";
        case TANKER_RESERVED_86:
            return "Tanker, Reserved (86)";
        case TANKER_RESERVED_87:
            return "Tanker, Reserved (87)";
        case TANKER_RESERVED_88:
            return "Tanker, Reserved (88)";
        case TANKER_NO_ADDITIONAL_INFO:
            return "Tanker, No additional info";
        case OTHER:
            return "Other Type";
        case OTHER_HAZARDOUS_A:
            return "Other Type, Hazardous A";
        case OTHER_HAZARDOUS_B:
            return "Other Type, Hazardous B";
        case OTHER_HAZARDOUS_C:
            return "Other Type, Hazardous C";
        case OTHER_HAZARDOUS_D:
            return "Other Type, Hazardous D";
        case OTHER_RESERVED_95:
            return "Other Type, Reserved (95)";
        case OTHER_RESERVED_96:
            return "Other Type, Reserved (96)";
        case OTHER_RESERVED_97:
            return "Other Type, Reserved (97)";
        case OTHER_RESERVED_98:
            return "Other Type, Reserved (98)";
        case OTHER_NO_ADDITIONAL_INFO:
            return "Other Type, no additional info";
        default:
            return "Unknown";
    }
}

Logger & operator << (Logger &logger, const AISShipType &shipType) {
    logger << shipType.name();

    return logger;
}
