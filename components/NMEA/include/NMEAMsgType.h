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

#ifndef NMEA_MSG_TYPE_H
#define NMEA_MSG_TYPE_H

#include <etl/string.h>

enum NMEAMsgType {
    NMEA_MSG_TYPE_UNKNOWN,
    NMEA_MSG_TYPE_DBK,
    NMEA_MSG_TYPE_DBS,
    NMEA_MSG_TYPE_DBT,
    NMEA_MSG_TYPE_GGA,
    NMEA_MSG_TYPE_GLL,
    NMEA_MSG_TYPE_GSA,
    NMEA_MSG_TYPE_GST,
    NMEA_MSG_TYPE_GSV,
    NMEA_MSG_TYPE_RMC,
    NMEA_MSG_TYPE_TXT,
    NMEA_MSG_TYPE_VDM,
    NMEA_MSG_TYPE_VDO,
    NMEA_MSG_TYPE_VTG
};

enum NMEAMsgType parseNMEAMsgType(const etl::istring &msgTypeStr);
const char *nmeaMsgTypeName(NMEAMsgType msgType);

#endif
