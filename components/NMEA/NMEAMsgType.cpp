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
#include "etl/string_view.h"

NMEAMsgType::NMEAMsgType() : value(UNKNOWN) {
}

NMEAMsgType::NMEAMsgType(Value value) : value(value) {
}

NMEAMsgType::NMEAMsgType(const etl::istring &msgTypeStr) {
    parse(msgTypeStr);
}

NMEAMsgType::NMEAMsgType(const etl::string_view &msgTypeStrView) {
    parse(msgTypeStrView);
}

void NMEAMsgType::parse(const etl::istring &msgTypeStr) {
    etl::string_view msgTypeStrView(msgTypeStr);
    parse(msgTypeStrView);
}

void NMEAMsgType::parse(const etl::string_view &msgTypeStrView) {
    if (msgTypeStrView == "DBK") {
        value = DBK;
    } else if (msgTypeStrView == "DBS") {
        value = DBS;
    } else if (msgTypeStrView == "DBT") {
        value = DBT;
    } else if (msgTypeStrView == "DPT") {
        value = DPT;
    } else if (msgTypeStrView == "GGA") {
        value = GGA;
    } else if (msgTypeStrView == "GLL") {
        value = GLL;
    } else if (msgTypeStrView == "GSA") {
        value = GSA;
    } else if (msgTypeStrView == "GST") {
        value = GST;
    } else if (msgTypeStrView == "GSV") {
        value = GSV;
    } else if (msgTypeStrView == "HDG") {
        value = HDG;
    } else if (msgTypeStrView == "MTW") {
        value = MTW;
    } else if (msgTypeStrView == "MWV") {
        value = MWV;
    } else if (msgTypeStrView == "RMC") {
        value = RMC;
    } else if (msgTypeStrView == "RSA") {
        value = RSA;
    } else if (msgTypeStrView == "TXT") {
        value = TXT;
    } else if (msgTypeStrView == "VDM") {
        value = VDM;
    } else if (msgTypeStrView == "VDO") {
        value = VDO;
    } else if (msgTypeStrView == "VHW") {
        value = VHW;
    } else if (msgTypeStrView == "VTG") {
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
        case HDG:
            return "HDG";
        case MTW:
            return "MTW";
        case MWV:
            return "MWV";
        case RMC:
            return "RMC";
        case RSA:
            return "RSA";
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
