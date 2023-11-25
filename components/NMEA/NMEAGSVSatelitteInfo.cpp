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

#include "NMEAGSVSatelitteInfo.h"
#include "NMEALine.h"
#include "NMEATalker.h"
#include "NMEAUInt16.h"
#include "NMEAInt8.h"
#include "NMEATenthsUInt16.h"
#include "NMEAUInt8.h"

#include "Logger.h"

bool NMEAGSVSatelitteInfo::extract(NMEALine &nmeaLine, NMEATalker &talker, bool &endOfInput) {
    if (nmeaLine.atEndOfLine()) {
        endOfInput = true;
        return true;
    }

    if (!id.extract(nmeaLine, talker, "GSV", "Satelitte ID")) {
        return false;
    }

    if (!elevation.extract(nmeaLine, talker, "GSV", "Elevation", false, -90, 90)) {
        return false;
    }

    if (!azimuth.extract(nmeaLine, talker, "GSV", "Azimuth")) {
        return false;
    }

    if (!signalToNoiseRatio.extract(nmeaLine, talker, "GSV", "Signal to Noise Ratio", true, 99)) {
        return false;
    }

    endOfInput = false;
    return true;
}

void NMEAGSVSatelitteInfo::log(Logger &logger) const {
    logger << "ID " << id << " Elevation " << elevation << "\xC2\xB0 Azimuth " << azimuth << " SNR "
           << signalToNoiseRatio << "db";
}
