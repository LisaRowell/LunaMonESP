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

#ifndef NMEA_INT8_H
#define NMEA_INT8_H

#include "NMEALine.h"
#include "NMEATalker.h"

// #include "DataModel/DataModelInt8Leaf.h"

#include "LoggableItem.h"
#include "Logger.h"

#include <etl/string_view.h>

#include <stdint.h>

class NMEAInt8 : public LoggableItem {
    private:
        int8_t value;
        bool valuePresent;

        bool set(const etl::string_view &valueView, bool optional, int8_t minValue,
                 int8_t maxValue);

    public:
        bool extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType,
                     const char *fieldName, bool optional = false, int8_t minValue = -128,
                     int8_t maxValue = 127);
        bool hasValue() const;
//        void publish(DataModelInt8Leaf &leaf) const;
        virtual void log(Logger &logger) const override;
};

#endif
