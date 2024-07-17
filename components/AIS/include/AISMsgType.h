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

#ifndef AIS_MSG_TYPE_H
#define AIS_MSG_TYPE_H

#include "etl/bit_stream.h"

#include <stddef.h>
#include <stdint.h>

class Logger;

class AISMsgType {
    public:
        enum Value : uint8_t {
            UNKNOWN,
            POS_REPORT_CLASS_A,
            POS_REPORT_CLASS_A_ASS_SCHED,
            POS_REPORT_CLASS_A_RESPONSE,
            BASE_STATION_REPORT,
            STATIC_AND_VOYAGE_DATA,
            BINARY_ADDRESSED_MSG,
            BINARY_ACKNOWLEDGE,
            BINARY_BROADCAST_MSG,
            STD_SAR_AIRCRAFT_POS_REPORT,
            UTC_AND_DATE_INQUIRY,
            UTC_AND_DATE_RESPONSE,
            ADDRESSED_SAFETY_RELATED_MSG,
            SAFETY_RELATED_ACKNOWLEDGEMENT,
            SAFETY_RELATED_BROADCAST_MSG,
            INTERROGATION,
            ASSIGNMENT_MODE_CMD,
            DGNSS_BINARY_BROADCAST_MSG,
            STANDARD_CLASS_B_POS_REPORT,
            EXTENDED_CLASS_B_POS_REPORT,
            DATA_LINK_MANAGEMENT,
            AID_TO_NAVIGATION_REPORT,
            CHANNEL_MANAGEMENT,
            GROUP_ASSIGNMENT_CMD,
            STATIC_DATA_REPORT,
            SINGLE_SLOT_BINARY_MSG,
            MULTI_SLOT_BINARY_MSG,
            POS_REPORT_LONG_RANGE_APPS,
            COUNT
        };

        AISMsgType();
        constexpr operator Value() const { return value; }
        void parse(etl::bit_stream_reader &streamReader);
        const char *name() const;
        explicit operator bool() const = delete;

        friend Logger & operator << (Logger &logger, const AISMsgType &aisMsgType);

    private:
        Value value;
};

#endif // AIS_MSG_TYPE_H
