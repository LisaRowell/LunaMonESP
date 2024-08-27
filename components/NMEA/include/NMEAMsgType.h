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

#ifndef NMEA_MSG_TYPE_H
#define NMEA_MSG_TYPE_H

#include "etl/string.h"
#include "etl/string_view.h"

#include <stdint.h>

class Logger;

class NMEAMsgType {
    public:
        enum Value : uint8_t {
            UNKNOWN,
            DBK,
            DBS,
            DBT,
            DPT,
            GGA,
            GLL,
            GSA,
            GST,
            GSV,
            HDG,
            MTW,
            MWV,
            RMC,
            RSA,
            TXT,
            VDM,
            VDO,
            VHW,
            VTG
        };

    private:
        Value value;

    public:
        NMEAMsgType();
        NMEAMsgType(Value value);
        NMEAMsgType(const etl::istring &msgTypeStr);
        NMEAMsgType(const etl::string_view &msgTypeStrView);
        void parse(const etl::istring &msgTypeStr);
        void parse(const etl::string_view &msgTypeStrView);
        constexpr operator Value() const { return value; }
        const char *name() const;
        explicit operator bool() const = delete;

        friend Logger & operator << (Logger &logger, const NMEAMsgType &nmeaMsgType);
};

#endif
