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

#ifndef NMEA_DATA_VALID_H
#define NMEA_DATA_VALID_H

#include "LoggableItem.h"

#include "etl/string_view.h"

class NMEALineWalker;
class NMEATalker;
class DataModelBoolLeaf;
class Logger;

class NMEADataValid : public LoggableItem {
    private:
        bool valid;

        bool set(etl::string_view &dataValidView, bool optional);

    public:
        NMEADataValid();
        bool extract(NMEALineWalker &lineWalker, NMEATalker &talker, const char *msgType,
                     bool optional = false);
        void publish(DataModelBoolLeaf &leaf) const;
        constexpr operator bool() const { return valid; }
        virtual void log(Logger &logger) const override;
};

#endif
