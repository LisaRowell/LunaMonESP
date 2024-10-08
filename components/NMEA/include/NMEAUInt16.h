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

#ifndef NMEA_UINT16_H
#define NMEA_UINT16_H

#include "LoggableItem.h"

#include "etl/string_view.h"

class NMEALineWalker;
class NMEATalker;
class DataModelUInt16Leaf;
class Logger;

class NMEAUInt16 : public LoggableItem {
    private:
        uint16_t value;
        bool valuePresent;

        bool set(const etl::string_view &valueView, bool optional, uint16_t maxValue);

    public:
        NMEAUInt16();
        bool extract(NMEALineWalker &lineWalker, NMEATalker &talker, const char *msgType,
                     const char *fieldName, bool optional = false, uint16_t maxValue = 0xffff);
        bool hasValue() const;
        operator uint16_t() const;
        uint16_t getValue() const;
        void publish(DataModelUInt16Leaf &leaf) const;
        virtual void log(Logger &logger) const override;
};

#endif
