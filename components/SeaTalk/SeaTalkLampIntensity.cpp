/*
 * This file is part of LunaMon (https://github.com/LisaRowell/LunaMonESP)
 * Copyright (C) 2024 Lisa Rowell
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

#include "SeaTalkLampIntensity.h"

#include "Logger.h"

#include <stdint.h>

SeaTalkLampIntensity::SeaTalkLampIntensity(uint8_t value) {
    this->value = (Value)value;
}

SeaTalkLampIntensity::SeaTalkLampIntensity(Value value) {
    this->value = value;
}

const char *SeaTalkLampIntensity::name() const {
    switch (value) {
        case L0:
            return "L0";
        case L1:
            return "L1";
        case L2:
            return "L2";
        case L3:
            return "L3";
        default:
            return "Unknown";
    }
}

bool SeaTalkLampIntensity::intensityValid() const {
    switch (value) {
        case L0:
        case L1:
        case L2:
        case L3:
            return true;
        default:
            return false;
    }
}

void SeaTalkLampIntensity::cycle() {
    switch (value) {
        case L0:
            value = L1;
            break;
        case L1:
            value = L2;
            break;
        case L2:
            value = L3;
            break;
        case L3:
        default:
            value = L0;
    }
}

Logger & operator << (Logger &logger, const SeaTalkLampIntensity &lampIntensity) {
    if (lampIntensity.intensityValid()) {
        logger << lampIntensity.name();
    } else {
        logger << "Unknown (" << Hex << (uint8_t)lampIntensity.value << ")";
    }

    return logger;
}
