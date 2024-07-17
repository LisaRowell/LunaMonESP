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

#ifndef AIS_SHIP_TYPE_H
#define AIS_SHIP_TYPE_H

#include "etl/bit_stream.h"

#include <stddef.h>
#include <stdint.h>

class Logger;

class AISShipType {
    public:
        enum Value : uint8_t {
            NOT_AVAILABLE,
            RESERVED_1,
            RESERVED_2,
            RESERVED_3,
            RESERVED_4,
            RESERVED_5,
            RESERVED_6,
            RESERVED_7,
            RESERVED_8,
            RESERVED_9,
            RESERVED_10,
            RESERVED_11,
            RESERVED_12,
            RESERVED_13,
            RESERVED_14,
            RESERVED_15,
            RESERVED_16,
            RESERVED_17,
            RESERVED_18,
            RESERVED_19,
            WING_IN_GROUND,
            WING_IN_GROUND_HAZARDOUS_A,
            WING_IN_GROUND_HAZARDOUS_B,
            WING_IN_GROUND_HAZARDOUS_C,
            WING_IN_GROUND_HAZARDOUS_D,
            WING_IN_GROUND_RESERVED_25,
            WING_IN_GROUND_RESERVED_26,
            WING_IN_GROUND_RESERVED_27,
            WING_IN_GROUND_RESERVED_28,
            WING_IN_GROUND_RESERVED_29,
            FISHING,
            TOWING,
            TOWING_LARGE_VESSEL,
            DREDGING_OR_UNDERWATER_OPS,
            DIVING_OPS,
            MILITARY_OPS,
            SAILING,
            PLEASURE_CRAFT,
            RESERVED_38,
            RESERVED_39,
            HIGH_SPEED_CRAFT,
            HIGH_SPEED_CRAFT_HAZARDOUS_A,
            HIGH_SPEED_CRAFT_HAZARDOUS_B,
            HIGH_SPEED_CRAFT_HAZARDOUS_C,
            HIGH_SPEED_CRAFT_HAZARDOUS_D,
            HIGH_SPEED_CRAFT_RESERVED_45,
            HIGH_SPEED_CRAFT_RESERVED_46,
            HIGH_SPEED_CRAFT_RESERVED_47,
            HIGH_SPEED_CRAFT_RESERVED_48,
            HIGH_SPEED_CRAFT_NO_ADDITIONAL_INFO,
            PILOT_VESSEL,
            SEARCH_AND_RESCUE_VESSEL,
            TUG,
            PORT_TENDER,
            ANTI_POLLUTION_EQUIPMENT,
            LAW_ENFORCEMENT,
            SPARE_LOCAL_VESSEL_56,
            SPARE_LOCAL_VESSEL_57,
            MEDICAL_TRANSPORT,
            NONCOMBATANT_SHIP,
            PASSENGER,
            PASSENGER_HAZARDOUS_A,
            PASSENGER_HAZARDOUS_B,
            PASSENGER_HAZARDOUS_C,
            PASSENGER_HAZARDOUS_D,
            PASSENGER_RESERVED_65,
            PASSENGER_RESERVED_66,
            PASSENGER_RESERVED_67,
            PASSENGER_RESERVED_68,
            PASSENGER_NO_ADDITIONAL_INFO,
            CARGO,
            CARGO_HAZARDOUS_A,
            CARGO_HAZARDOUS_B,
            CARGO_HAZARDOUS_C,
            CARGO_HAZARDOUS_D,
            CARGO_RESERVED_75,
            CARGO_RESERVED_76,
            CARGO_RESERVED_77,
            CARGO_RESERVED_78,
            CARGO_NO_ADDITIONAL_INFO,
            TANKER,
            TANKER_HAZARDOUS_A,
            TANKER_HAZARDOUS_B,
            TANKER_HAZARDOUS_C,
            TANKER_HAZARDOUS_D,
            TANKER_RESERVED_85,
            TANKER_RESERVED_86,
            TANKER_RESERVED_87,
            TANKER_RESERVED_88,
            TANKER_NO_ADDITIONAL_INFO,
            OTHER,
            OTHER_HAZARDOUS_A,
            OTHER_HAZARDOUS_B,
            OTHER_HAZARDOUS_C,
            OTHER_HAZARDOUS_D,
            OTHER_RESERVED_95,
            OTHER_RESERVED_96,
            OTHER_RESERVED_97,
            OTHER_RESERVED_98,
            OTHER_NO_ADDITIONAL_INFO,
            COUNT
      };

    private:
        Value value;

    public:
        AISShipType();
        AISShipType(etl::bit_stream_reader &streamReader);
        constexpr operator Value() const { return value; }
        const char *name() const;
        explicit operator bool() const = delete;

        friend Logger & operator << (Logger &logger, const AISShipType &shipType);
};

#endif // AIS_SHIP_TYPE_H
