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

#ifndef NMEA_EPFD_FIX_TYPE_H
#define NMEA_EPFD_FIX_TYPE_H

#include "LoggableItem.h"

#include "etl/bit_stream.h"

class NMEAEPFDFixType : public LoggableItem {
    private:
        enum EPFDFixTypeEnum {
            EPFD_FIX_TYPE_UNDEFINED,
            EPFD_FIX_TYPE_GPS,
            EPFD_FIX_TYPE_GLONASS,
            EPFD_FIX_TYPE_COMBINED_GPS_GLONASS,
            EPFD_FIX_TYPE_LORAN_C,
            EPFD_FIX_TYPE_CHAYKA,
            EPFD_FIX_TYPE_INTEGRATED_NAV_SYS,
            EPFD_FIX_TYPE_SURVEYED,
            EPFD_FIX_TYPE_GALILEO,
            EPFD_FIX_TYPE_RESERVED_9,
            EPFD_FIX_TYPE_RESERVED_10,
            EPFD_FIX_TYPE_RESERVED_11,
            EPFD_FIX_TYPE_RESERVED_12,
            EPFD_FIX_TYPE_RESERVED_13,
            EPFD_FIX_TYPE_RESERVED_14,
            EPFD_FIX_TYPE_INTERNAL_GNSS
        };

        enum EPFDFixTypeEnum type;

        virtual void log(Logger &logger) const override;
        const char *toString() const;

    public:
        void parse(etl::bit_stream_reader &streamReader);
};

#endif // NMEA_EPFD_FIX_TYPE_H
