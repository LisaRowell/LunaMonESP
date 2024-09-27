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

#include "RMTSymbolWriter.h"

#include "driver/rmt_types.h"

#include <stddef.h>
#include <stdint.h>

RMTSymbolWriter::RMTSymbolWriter(rmt_symbol_word_t *symbols, size_t symbolsFree)
    : currentSymbol(symbols),
      currentSide(0),
      symbolsFree(symbolsFree),
      lastSymbol(nullptr),
      lastSide(0),
      _written(0) {
}

bool RMTSymbolWriter::addLevelDuration(uint16_t level, uint16_t duration) {
    // If the last value put in was the same level as the current one, we can add this duration to
    // it.
    if (lastSymbol != nullptr) {
        if (lastSide == 0) {
            if (lastSymbol->level0 == level) {
                lastSymbol->duration0 += duration;
                return false;
            }
        } else {
            if (lastSymbol->level1 == level) {
                lastSymbol->duration1 += duration;
                return symbolsFree == 0;
            }
        }
    }

    // We are adding a new level since it represents a bit flip
    if (currentSide == 0) {
        currentSymbol->level0 = level;
        currentSymbol->duration0 = duration;
        symbolsFree--;
        lastSymbol = currentSymbol;
        lastSide = currentSide;
        currentSide = 1;
        _written++;
        return false;
    } else {
        currentSymbol->level1 = level;
        currentSymbol->duration1 = duration;
        lastSide = currentSide;
        currentSymbol++;
        currentSide = 0;
        return symbolsFree == 0;
    }
}

void RMTSymbolWriter::finish() {
    if (currentSide == 1) {
        currentSymbol->level1 = 1;
        currentSymbol->duration1 = 0;
    }
}

size_t RMTSymbolWriter::written() {
    return _written;
}
