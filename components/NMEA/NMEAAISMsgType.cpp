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

#include "NMEAAISMsgType.h"

#include <stdint.h>

enum NMEAAISMsgType parseNMEAAISMsgType(uint8_t msgTypeCode) {
    if (msgTypeCode < NMEA_AIS_MSG_TYPE_COUNT) {
        return (enum NMEAAISMsgType)msgTypeCode;
    } else {
        return NMEA_AIS_MSG_TYPE_UNKNOWN;
    }
}

const char *nmeaAISMsgTypeName(NMEAAISMsgType aisMsgType) {
    switch (aisMsgType) {
        case NMEA_AIS_MSG_TYPE_POS_REPORT_CLASS_A:
            return "Position Report Class A";
        case NMEA_AIS_MSG_TYPE_POS_REPORT_CLASS_A_ASS_SCHED:
            return "Position Report Class A (Assigned schedule)";
        case NMEA_AIS_MSG_TYPE_POS_REPORT_CLASS_A_RESPONSE:
            return "Position Report Class A (Response to interrogation)";
        case NMEA_AIS_MSG_TYPE_BASE_STATION_REPORT:
            return "Base Station Report";
        case NMEA_AIS_MSG_TYPE_STATIC_AND_VOYAGE_DATA:
            return "Static and Voyage Related Data";
        case NMEA_AIS_MSG_TYPE_BINARY_ADDRESSED_MSG:
            return "Binary Addressed Message";
        case NMEA_AIS_MSG_TYPE_BINARY_ACKNOWLEDGE:
            return "Binary Acknowledge";
        case NMEA_AIS_MSG_TYPE_BINARY_BROADCAST_MSG:
            return "Binary Broadcast Message";
        case NMEA_AIS_MSG_TYPE_STD_SAR_AIRCRAFT_POS_REPORT:
            return "Standard SAR Aircraft Position Report";
        case NMEA_AIS_MSG_TYPE_UTC_AND_DATE_INQUIRY:
            return "UTC and Date Inquiry";
        case NMEA_AIS_MSG_TYPE_UTC_AND_DATE_RESPONSE:
            return "UTC and Date Response";
        case NMEA_AIS_MSG_TYPE_ADDRESSED_SAFETY_RELATED_MSG:
            return "Addressed Safety Related Message";
        case NMEA_AIS_MSG_TYPE_SAFETY_RELATED_ACKNOWLEDGEMENT:
            return "Safety Related Acknowledgement";
        case NMEA_AIS_MSG_TYPE_SAFETY_RELATED_BROADCAST_MSG:
            return "Safety Related Broadcast Message";
        case NMEA_AIS_MSG_TYPE_INTERROGATION:
            return "Interrogation";
        case NMEA_AIS_MSG_TYPE_ASSIGNMENT_MODE_CMD:
            return "Assignment Mode Command";
        case NMEA_AIS_MSG_TYPE_DGNSS_BINARY_BROADCAST_MSG:
            return "DGNSS Binary Broadcast Message";
        case NMEA_AIS_MSG_TYPE_STANDARD_CLASS_B_POS_REPORT:
            return "Standard Class B CS Position Report";
        case NMEA_AIS_MSG_TYPE_EXTENDED_CLASS_B_POS_REPORT:
            return "Extended Class B Equipment Position Report";
        case NMEA_AIS_MSG_TYPE_DATA_LINK_MANAGEMENT:
            return "Data Link Management";
        case NMEA_AIS_MSG_TYPE_AID_TO_NAVIGATION_REPORT:
            return "Aid-to-Navigation Report";
        case NMEA_AIS_MSG_TYPE_CHANNEL_MANAGEMENT:
            return "Channel Management";
        case NMEA_AIS_MSG_TYPE_GROUP_ASSIGNMENT_CMD:
            return "Group Assignment Command";
        case NMEA_AIS_MSG_TYPE_STATIC_DATA_REPORT:
            return "Static Data Report";
        case NMEA_AIS_MSG_TYPE_SINGLE_SLOT_BINARY_MSG:
            return "Single Slot Binary Message";
        case NMEA_AIS_MSG_TYPE_MULTI_SLOT_BINARY_MSG:
            return "Multiple Slot Binary Message With Communications State";
        case NMEA_AIS_MSG_TYPE_POS_REPORT_LONG_RANGE_APPS:
            return "Position Report For Long-Range Applications";
        case NMEA_AIS_MSG_TYPE_UNKNOWN:
        default:
            return "Unknown";
    }
}
