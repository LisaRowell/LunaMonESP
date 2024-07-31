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

#ifndef NMEA_TENTHS_INT16_H
#define NMEA_TENTHS_INT16_H

#include "TenthsInt16.h"

#include "LoggableItem.h"

#include "etl/string_view.h"

#include <stdint.h>

class NMEALine;
class NMEATalker;
class DataModelTenthsInt16Leaf;
class Logger;

class NMEATenthsInt16 : public LoggableItem {
    private:
        TenthsInt16 value;
        bool valuePresent;

        bool set(const etl::string_view &valueView, bool optional);

    public:
        NMEATenthsInt16();
        bool extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType,
                     const char *fieldName, bool optional = false);
        void publish(DataModelTenthsInt16Leaf &leaf) const;
        virtual void log(Logger &logger) const override;
};

#endif
