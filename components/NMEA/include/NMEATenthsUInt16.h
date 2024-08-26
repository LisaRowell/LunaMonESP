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

#ifndef NMEA_TENTHS_UINT16_H
#define NMEA_TENTHS_UINT16_H

#include "TenthsUInt16.h"

#include "LoggableItem.h"

#include "etl/string_view.h"

class NMEALineWalker;
class NMEATalker;
class DataModelTenthsUInt16Leaf;
class Logger;

class NMEATenthsUInt16 : public LoggableItem {
    private:
        TenthsUInt16 value;
        bool valuePresent;

    public:
        NMEATenthsUInt16();
        bool set(const etl::string_view &valueView, bool optional);
        bool extract(NMEALineWalker &lineWalker, NMEATalker &talker, const char *msgType,
                     const char *fieldName, bool optional = false);
        bool hasValue() const;
        constexpr operator TenthsUInt16() const { return value; }
        void publish(DataModelTenthsUInt16Leaf &leaf) const;
        virtual void log(Logger &logger) const override;
};

#endif
