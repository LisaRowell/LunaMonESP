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

#ifndef NMEA_GSV_SATELLITE_INFO_H
#define NMEA_GSV_SATELLITE_INFO_H

#include "NMEAUInt16.h"
#include "NMEAInt8.h"
#include "NMEATenthsUInt16.h"
#include "NMEAUInt8.h"
#include "NMEALine.h"

#include "LoggableItem.h"

class NMEAGSVSatelitteInfo : public LoggableItem {
    private:
        NMEAUInt16 id;
        NMEAInt8 elevation;
        NMEATenthsUInt16 azimuth;
        NMEAUInt8 signalToNoiseRatio;

    public:
        bool extract(NMEALineWalker &lineWalker, NMEATalker &talker, bool &endOfInput);
        virtual void log(Logger &logger) const override;

    friend class NMEADataModelBridge;
};

#endif
