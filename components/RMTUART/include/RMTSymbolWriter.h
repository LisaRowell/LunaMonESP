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

#ifndef RMT_SYMBOL_WRITER_H
#define RMT_SYMBOL_WRITER_H

#include "driver/rmt_types.h"

#include <stddef.h>
#include <stdint.h>

class RMTSymbolWriter {
    private:
        rmt_symbol_word_t *currentSymbol;
        uint8_t currentSide;
        size_t symbolsFree;
        rmt_symbol_word_t *lastSymbol;
        uint8_t lastSide;
        size_t _written;

    public:
        RMTSymbolWriter(rmt_symbol_word_t *symbols, size_t symbolsFree);
        bool addLevelDuration(uint16_t level, uint16_t duration);
        void finish();
        size_t written();
};

#endif // RMT_SYMBOL_WRITER_H
