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

#ifndef RMT_UART_BIT_STREAMER_H
#define RMT_UART_BIT_STREAMER_H

#include "InterfaceParams.h"

#include <stddef.h>
#include <stdint.h>

class RMTUARTBitStreamer {
    private:
        enum State : uint8_t {
            IN_RESET,
            IN_START_BIT,
            IN_CHARACTER_BITS,
            IN_PARITY_BIT,
            IN_STOP_BITS,
            COMPLETED
        };

        size_t dataBitsPerCharacter;
        uint16_t bitDuration;
        InterfaceParity parity;
        uint16_t stopBitDuration;
        State state;
        union {
            const uint16_t *word;
            const uint8_t *byte;
        } dataPos;
        size_t bytesRemaining;
        uint16_t characterBits;
        size_t bitsRemainingInChar;
        uint16_t parityValue;

        void streamStartBit(uint16_t &level, uint16_t &duration);
        void streamCharacterBit(uint16_t &level, uint16_t &duration);
        void streamParityBit(uint16_t &level, uint16_t &duration);
        bool streamStopBits(uint16_t &level, uint16_t &duration);

    public:
        RMTUARTBitStreamer(uint32_t baudRate, InterfaceDataWidth dataWidth, InterfaceParity parity,
                           InterfaceStopBits stopBits, uint32_t resolutionHz);
        void reset();
        void start(const void *data, size_t dataSize);
        bool nextBit(uint16_t &level, uint16_t &duration);
        bool midStream();
};

#endif // RMT_UART_BIT_STREAMER_H
