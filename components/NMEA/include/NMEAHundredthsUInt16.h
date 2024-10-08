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

#ifndef NMEA_HUNDREDTHS_UINT16_H
#define NMEA_HUNDREDTHS_UINT16_H

#include "NMEALineWalker.h"
#include "NMEATalker.h"

#include "HundredthsUInt16.h"

#include "LoggableItem.h"
#include "Logger.h"

#include "etl/string_view.h"

class DataModelHundredthsUInt16Leaf;

class NMEAHundredthsUInt16 : public LoggableItem {
    private:
        HundredthsUInt16 value;

        bool set(const etl::string_view &valueView);

    public:
        bool extract(NMEALineWalker &lineWalker, NMEATalker &talker, const char *msgType,
                     const char *fieldName);
        constexpr operator HundredthsUInt16() const { return value; }
        void publish(DataModelHundredthsUInt16Leaf &leaf) const;
        virtual void log(Logger &logger) const override;
};

#endif
