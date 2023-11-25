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

#ifndef NMEA_DATA_VALID_H
#define NMEA_DATA_VALID_H

#include "NMEALine.h"
#include "NMEATalker.h"

// #include "DataModel/DataModelBoolLeaf.h"

#include "LoggableItem.h"
#include "Logger.h"

#include <etl/string_view.h>

class NMEADataValid : public LoggableItem {
    private:
        bool valid;

        bool set(etl::string_view &dataValidView);

    public:
        NMEADataValid();
        bool extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType);
//        void publish(DataModelBoolLeaf &leaf) const;
        virtual void log(Logger &logger) const override;
};

#endif
