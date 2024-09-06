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

#ifndef RMT_SYMBOL_READER_H
#define RMT_SYMBOL_READER_H

#include "driver/rmt_types.h"
#include "driver/rmt_rx.h"

#include <stddef.h>
#include <stdint.h>

class RMTSymbolReader {
    private:
        const rmt_symbol_word_t *currentWord;
        size_t remainingWords;
        bool onFirstOfPair;

    public:
        RMTSymbolReader(const rmt_symbol_word_t *symbols, size_t count);
        bool nextSymbol(uint16_t &duration, uint16_t &level);
};

#endif // RMT_SYMBOL_READER_H
