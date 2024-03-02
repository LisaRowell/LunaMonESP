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

#ifndef NMEA_FAA_MODE_INDICATOR_H
#define NMEA_FAA_MODE_INDICATOR_H

#include "LoggableItem.h"

#include "etl/string_view.h"

class NMEALine;
class NMEATalker;
class DataModelStringLeaf;
class Logger;

class NMEAFAAModeIndicator : public LoggableItem {
    private:
        enum FAAMode {
            FAA_MODE_NONE,
            FAA_MODE_AUTONOMOUS,
            FAA_MODE_CAUTION,
            FAA_MODE_DIFFERENTIAL,
            FAA_MODE_ESTIMATED,
            FAA_MODE_RTK_FLOAT,
            FAA_MODE_MANUAL,
            FAA_MODE_DATA_NOT_VALID,
            FAA_MODE_PRECISE,
            FAA_MODE_RTK_INTEGER,
            FAA_MODE_SIMULATED,
            FAA_MODE_UNSAFE
        };
        FAAMode faaMode;

        bool set(etl::string_view &faaModeView);

    public:
        NMEAFAAModeIndicator();
        bool extract(NMEALine &nmeaLine, NMEATalker &talker, const char *msgType);
        bool hasValue() const;
        void publish(DataModelStringLeaf &leaf) const;
        virtual void log(Logger &logger) const override;
};

#endif
