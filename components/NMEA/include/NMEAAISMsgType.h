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

#ifndef NMEA_AIS_MSG_TYPE_H
#define NMEA_AIS_MSG_TYPE_H

#include <stdint.h>

enum NMEAAISMsgType {
    NMEA_AIS_MSG_TYPE_UNKNOWN,
    NMEA_AIS_MSG_TYPE_POS_REPORT_CLASS_A,
    NMEA_AIS_MSG_TYPE_POS_REPORT_CLASS_A_ASS_SCHED,
    NMEA_AIS_MSG_TYPE_POS_REPORT_CLASS_A_RESPONSE,
    NMEA_AIS_MSG_TYPE_BASE_STATION_REPORT,
    NMEA_AIS_MSG_TYPE_STATIC_AND_VOYAGE_DATA,
    NMEA_AIS_MSG_TYPE_BINARY_ADDRESSED_MSG,
    NMEA_AIS_MSG_TYPE_BINARY_ACKNOWLEDGE,
    NMEA_AIS_MSG_TYPE_BINARY_BROADCAST_MSG,
    NMEA_AIS_MSG_TYPE_STD_SAR_AIRCRAFT_POS_REPORT,
    NMEA_AIS_MSG_TYPE_UTC_AND_DATE_INQUIRY,
    NMEA_AIS_MSG_TYPE_UTC_AND_DATE_RESPONSE,
    NMEA_AIS_MSG_TYPE_ADDRESSED_SAFETY_RELATED_MSG,
    NMEA_AIS_MSG_TYPE_SAFETY_RELATED_ACKNOWLEDGEMENT,
    NMEA_AIS_MSG_TYPE_SAFETY_RELATED_BROADCAST_MSG,
    NMEA_AIS_MSG_TYPE_INTERROGATION,
    NMEA_AIS_MSG_TYPE_ASSIGNMENT_MODE_CMD,
    NMEA_AIS_MSG_TYPE_DGNSS_BINARY_BROADCAST_MSG,
    NMEA_AIS_MSG_TYPE_STANDARD_CLASS_B_POS_REPORT,
    NMEA_AIS_MSG_TYPE_EXTENDED_CLASS_B_POS_REPORT,
    NMEA_AIS_MSG_TYPE_DATA_LINK_MANAGEMENT,
    NMEA_AIS_MSG_TYPE_AID_TO_NAVIGATION_REPORT,
    NMEA_AIS_MSG_TYPE_CHANNEL_MANAGEMENT,
    NMEA_AIS_MSG_TYPE_GROUP_ASSIGNMENT_CMD,
    NMEA_AIS_MSG_TYPE_STATIC_DATA_REPORT,
    NMEA_AIS_MSG_TYPE_SINGLE_SLOT_BINARY_MSG,
    NMEA_AIS_MSG_TYPE_MULTI_SLOT_BINARY_MSG,
    NMEA_AIS_MSG_TYPE_POS_REPORT_LONG_RANGE_APPS,
    NMEA_AIS_MSG_TYPE_COUNT
};

enum NMEAAISMsgType parseNMEAAISMsgType(uint8_t msgTypeCode);
const char *nmeaAISMsgTypeName(NMEAAISMsgType aisMsgType);

#endif // NMEA_AIS_MSG_TYPE_H
