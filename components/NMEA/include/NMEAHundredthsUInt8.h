/*
 * This file is part of LunaMon (https://github.com/LisaRowell/LunaMonESP)
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

#ifndef NMEA_HUNDREDTHS_UINT8_H
#define NMEA_HUNDREDTHS_UINT8_H

#include "NMEALine.h"
#include "NMEATalker.h"

// #include "DataModel/DataModelHundredthsUInt8Leaf.h"

#include "LoggableItem.h"
#include "Logger.h"

#include "etl/string_view.h"

#include <stdint.h>

class NMEAHundredthsUInt8 : public LoggableItem {
    private:
        uint8_t wholeNumber;
        uint8_t hundredths;

        bool set(const etl::string_view &decimalString);

    public:
        bool extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType,
                     const char *fieldName);
//        void publish(DataModelHundredthsUInt8Leaf &leaf) const;
        virtual void log(Logger &logger) const override;
};

#endif
