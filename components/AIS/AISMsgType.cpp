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

#include "AISMsgType.h"

#include "Logger.h"

#include "etl/bit_stream.h"

#include <stddef.h>
#include <stdint.h>

AISMsgType::AISMsgType() {
    value = UNKNOWN;
}

void AISMsgType::parse(etl::bit_stream_reader &streamReader) {
    const uint8_t messageTypeCode = etl::read_unchecked<uint8_t>(streamReader, 6);

    if (messageTypeCode < COUNT) {
        value = (enum AISMsgType::Value)messageTypeCode;
    } else {
        value = UNKNOWN;
    }
}

const char *AISMsgType::name() const {
    switch (value) {
        case POS_REPORT_CLASS_A:
            return "Position Report Class A";
        case POS_REPORT_CLASS_A_ASS_SCHED:
            return "Position Report Class A (Assigned schedule)";
        case POS_REPORT_CLASS_A_RESPONSE:
            return "Position Report Class A (Response to interrogation)";
        case BASE_STATION_REPORT:
            return "Base Station Report";
        case STATIC_AND_VOYAGE_DATA:
            return "Static and Voyage Related Data";
        case BINARY_ADDRESSED_MSG:
            return "Binary Addressed Message";
        case BINARY_ACKNOWLEDGE:
            return "Binary Acknowledge";
        case BINARY_BROADCAST_MSG:
            return "Binary Broadcast Message";
        case STD_SAR_AIRCRAFT_POS_REPORT:
            return "Standard SAR Aircraft Position Report";
        case UTC_AND_DATE_INQUIRY:
            return "UTC and Date Inquiry";
        case UTC_AND_DATE_RESPONSE:
            return "UTC and Date Response";
        case ADDRESSED_SAFETY_RELATED_MSG:
            return "Addressed Safety Related Message";
        case SAFETY_RELATED_ACKNOWLEDGEMENT:
            return "Safety Related Acknowledgement";
        case SAFETY_RELATED_BROADCAST_MSG:
            return "Safety Related Broadcast Message";
        case INTERROGATION:
            return "Interrogation";
        case ASSIGNMENT_MODE_CMD:
            return "Assignment Mode Command";
        case DGNSS_BINARY_BROADCAST_MSG:
            return "DGNSS Binary Broadcast Message";
        case STANDARD_CLASS_B_POS_REPORT:
            return "Standard Class B CS Position Report";
        case EXTENDED_CLASS_B_POS_REPORT:
            return "Extended Class B Equipment Position Report";
        case DATA_LINK_MANAGEMENT:
            return "Data Link Management";
        case AID_TO_NAVIGATION_REPORT:
            return "Aid-to-Navigation Report";
        case CHANNEL_MANAGEMENT:
            return "Channel Management";
        case GROUP_ASSIGNMENT_CMD:
            return "Group Assignment Command";
        case STATIC_DATA_REPORT:
            return "Static Data Report";
        case SINGLE_SLOT_BINARY_MSG:
            return "Single Slot Binary Message";
        case MULTI_SLOT_BINARY_MSG:
            return "Multiple Slot Binary Message With Communications State";
        case POS_REPORT_LONG_RANGE_APPS:
            return "Position Report For Long-Range Applications";
        case UNKNOWN:
        default:
            return "Unknown";
    }
}

Logger & operator << (Logger &logger, const AISMsgType &aisMsgType) {
    logger << aisMsgType.name();

    return logger;
}
