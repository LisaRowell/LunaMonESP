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

#ifndef NMEA_UINT32_H
#define NMEA_UINT32_H

#include "LoggableItem.h"

#include "etl/string_view.h"

class NMEALine;
class NMEATalker;
class NMEAMsgType;
class DataModelUInt32Leaf;
class Logger;

class NMEAUInt32 : public LoggableItem {
    private:
        uint32_t value;
        bool valuePresent;

        bool set(const etl::string_view &valueView, bool optional, uint32_t maxValue);

    public:
        bool extract(NMEALine &nmeaLine, const NMEATalker &talker, const char *msgTypeName,
                     const char *fieldName, bool optional = false, uint32_t maxValue = 0xffffffff);
        bool hasValue() const;
        operator uint32_t() const;
        uint32_t getValue() const;
        void publish(DataModelUInt32Leaf &leaf) const;
        virtual void log(Logger &logger) const override;
};

#endif // NMEA_UINT32_H
