/*
 * This file is part of LunaMon (https://github.com/LisaRowell/LunaMon)
 * Copyright (C) 2021-2023 Lisa Rowell
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

#include "NMEAMsgType.h"

#include "Error.h"

#include <etl/string.h>

enum NMEAMsgType parseNMEAMsgType(const etl::istring &msgTypeStr) {
    if (msgTypeStr == "DBK") {
        return NMEA_MSG_TYPE_DBK;
    } else if (msgTypeStr == "DBS") {
        return NMEA_MSG_TYPE_DBS;
    } else if (msgTypeStr == "DBT") {
        return NMEA_MSG_TYPE_DBT;
    } else if (msgTypeStr == "GGA") {
        return NMEA_MSG_TYPE_GGA;
    } else if (msgTypeStr == "GLL") {
        return NMEA_MSG_TYPE_GLL;
    } else if (msgTypeStr == "GSA") {
        return NMEA_MSG_TYPE_GSA;
    } else if (msgTypeStr == "GST") {
        return NMEA_MSG_TYPE_GST;
    } else if (msgTypeStr == "GSV") {
        return NMEA_MSG_TYPE_GSV;
    } else if (msgTypeStr == "RMC") {
        return NMEA_MSG_TYPE_RMC;
    } else if (msgTypeStr == "TXT") {
        return NMEA_MSG_TYPE_TXT;
    } else if (msgTypeStr == "VDM") {
        return NMEA_MSG_TYPE_VDM;
    } else if (msgTypeStr == "VDO") {
        return NMEA_MSG_TYPE_VDO;
    } else if (msgTypeStr == "VTG") {
        return NMEA_MSG_TYPE_VTG;
    } else {
        return NMEA_MSG_TYPE_UNKNOWN;
    }
}

const char *nmeaMsgTypeName(NMEAMsgType msgType) {
    switch (msgType) {
        case NMEA_MSG_TYPE_UNKNOWN:
            return "Unknown";
        case NMEA_MSG_TYPE_DBK:
            return "DBK";
        case NMEA_MSG_TYPE_DBS:
            return "DBS";
        case NMEA_MSG_TYPE_DBT:
            return "DBT";
        case NMEA_MSG_TYPE_GGA:
            return "GGA";
        case NMEA_MSG_TYPE_GLL:
            return "GLL";
        case NMEA_MSG_TYPE_GSA:
            return "GSA";
        case NMEA_MSG_TYPE_GST:
            return "GST";
        case NMEA_MSG_TYPE_GSV:
            return "GSV";
        case NMEA_MSG_TYPE_RMC:
            return "RMC";
        case NMEA_MSG_TYPE_TXT:
            return "TXT";
        case NMEA_MSG_TYPE_VDM:
            return "VDM";
        case NMEA_MSG_TYPE_VDO:
            return "VDO";
        case NMEA_MSG_TYPE_VTG:
            return "VTG";
        default:
            fatalError("Missing NMEA message type in name function");
    }
}
