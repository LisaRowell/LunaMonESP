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

#include "RMTSymbolReader.h"

#include "Logger.h"
#include "Error.h"

#include "driver/rmt_types.h"
#include "driver/rmt_rx.h"

#include <stddef.h>
#include <stdint.h>

// At least in ESP-IDF v5.3-dev-687-g48d732f49c with DMA (no other combinations tested), the
// num_symbols field in the rmt_rx_done_event_data_t structure returned to the ISR level callback
// actually contains the number of symbol words. Since there are two symbols per words, this is off
// by a factor of two. In the last symbol there may be a symbol with a 0 duration (duration1).
RMTSymbolReader::RMTSymbolReader(const rmt_symbol_word_t *symbols, size_t count)
    : currentWord(symbols),
      remainingWords(count),
      onFirstOfPair(true) {
}

bool RMTSymbolReader::nextSymbol(uint16_t &duration, uint16_t &level) {
    if (remainingWords) {
        if (onFirstOfPair) {
            if (currentWord->duration0 == 0) {
                logger() << logWarnRMTUART << "Unexpected 0 duration in duration0. Remaining words"
                         << remainingWords << " duration1 " << currentWord->duration1 << eol;
                return false;
            }

            duration = currentWord->duration0;
            level = currentWord->level0;
            onFirstOfPair = false;
            return true;
        } else {
            if (currentWord->duration1 == 0) {
                remainingWords = 0;
                return false;
            } else {
                duration = currentWord->duration1;
                level = currentWord->level1;
                onFirstOfPair = true;
                currentWord++;
                remainingWords--;
                return true;
            }
        }
    } else {
        return false;
    }
}
