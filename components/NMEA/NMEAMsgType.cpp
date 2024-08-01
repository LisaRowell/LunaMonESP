/*
 * This file is part of LunaMon (https://github.com/LisaRowell/LunaMonESP)
 * Copyright (C) 2021-2024 Lisa Rowell
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

#include "Logger.h"
#include "Error.h"

#include "etl/string.h"

NMEAMsgType::NMEAMsgType() : value(UNKNOWN) {
}

NMEAMsgType::NMEAMsgType(Value value) : value(value) {
}

NMEAMsgType::NMEAMsgType(const etl::istring &msgTypeStr) {
    parse(msgTypeStr);
}

void NMEAMsgType::parse(const etl::istring &msgTypeStr) {
    if (msgTypeStr == "DBK") {
        value = DBK;
    } else if (msgTypeStr == "DBS") {
        value = DBS;
    } else if (msgTypeStr == "DBT") {
        value = DBT;
    } else if (msgTypeStr == "DPT") {
        value = DPT;
    } else if (msgTypeStr == "GGA") {
        value = GGA;
    } else if (msgTypeStr == "GLL") {
        value = GLL;
    } else if (msgTypeStr == "GSA") {
        value = GSA;
    } else if (msgTypeStr == "GST") {
        value = GST;
    } else if (msgTypeStr == "GSV") {
        value = GSV;
    } else if (msgTypeStr == "MTW") {
        value = MTW;
    } else if (msgTypeStr == "MWV") {
        value = MWV;
    } else if (msgTypeStr == "RMC") {
        value = RMC;
    } else if (msgTypeStr == "TXT") {
        value = TXT;
    } else if (msgTypeStr == "VDM") {
        value = VDM;
    } else if (msgTypeStr == "VDO") {
        value = VDO;
    } else if (msgTypeStr == "VHW") {
        value = VHW;
    } else if (msgTypeStr == "VTG") {
        value = VTG;
    } else {
        value = UNKNOWN;
    }
}

const char *NMEAMsgType::name() const {
    switch (value) {
        case UNKNOWN:
            return "Unknown";
        case DBK:
            return "DBK";
        case DBS:
            return "DBS";
        case DBT:
            return "DBT";
        case DPT:
            return "DPT";
        case GGA:
            return "GGA";
        case GLL:
            return "GLL";
        case GSA:
            return "GSA";
        case GST:
            return "GST";
        case GSV:
            return "GSV";
        case MTW:
            return "MTW";
        case MWV:
            return "MWV";
        case RMC:
            return "RMC";
        case TXT:
            return "TXT";
        case VDM:
            return "VDM";
        case VDO:
            return "VDO";
        case VHW:
            return "VHW";
        case VTG:
            return "VTG";
    }
    fatalError("Missing NMEAMsgType in name function");
}

Logger & operator << (Logger &logger, const NMEAMsgType &nmeaMsgType) {
    logger << nmeaMsgType.name();

    return logger;
}
