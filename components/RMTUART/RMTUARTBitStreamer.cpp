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

#include "RMTUARTBitStreamer.h"

#include "InterfaceParams.h"

#include "Error.h"

#include <stddef.h>
#include <stdint.h>

RMTUARTBitStreamer::RMTUARTBitStreamer(uint32_t baudRate, InterfaceDataWidth dataWidth,
                                       InterfaceParity parity, InterfaceStopBits stopBits,
                                       uint32_t resolutionHz)
    : parity(parity) {
    dataBitsPerCharacter = interfaceDataWidthBits(dataWidth);
    bitDuration = resolutionHz / baudRate;
    stopBitDuration = ((bitDuration + 1) / 2) * interfaceHalfStopBits(stopBits);

    reset();
}

void RMTUARTBitStreamer::reset() {
    state = IN_RESET;
}

void RMTUARTBitStreamer::start(const void *data, size_t dataSize) {
    dataPos.byte = (const uint8_t *)data;
    bytesRemaining = dataSize;
    state = IN_START_BIT;
}

bool RMTUARTBitStreamer::nextBit(uint16_t &level, uint16_t &duration) {
    switch (state) {
        case IN_START_BIT:
            streamStartBit(level, duration);
            return true;

        case IN_CHARACTER_BITS:
            streamCharacterBit(level, duration);
            return true;

        case IN_PARITY_BIT:
            streamParityBit(level, duration);
            return true;

        case IN_STOP_BITS:
            return streamStopBits(level, duration);

        case IN_RESET:
            fatalError("RMT UART encoder nextBit called in reset state");

        case COMPLETED:
            fatalError("RMT UART encoder nextBit called in completed state");

        default:
            fatalError("RMT UART encoder running in bad state");
    }
}

void RMTUARTBitStreamer::streamStartBit(uint16_t &level, uint16_t &duration) {
    level = 0;
    duration = bitDuration;

    state = IN_CHARACTER_BITS;
    if (dataBitsPerCharacter > 8) {
        characterBits = *(dataPos.word++);
        bytesRemaining -= sizeof(uint16_t);
    } else {
        characterBits = *(dataPos.byte++);
        bytesRemaining -= sizeof(uint8_t);
    }
    bitsRemainingInChar = dataBitsPerCharacter;
    parityValue = 0;
}

void RMTUARTBitStreamer::streamCharacterBit(uint16_t &level, uint16_t &duration) {
    uint16_t bit = characterBits & 0x0001;
    parityValue ^= bit;

    level = bit;
    duration = bitDuration;

    bitsRemainingInChar--;
    characterBits >>= 1;
    if (bitsRemainingInChar == 0) {
        if (parity == PARITY_NONE) {
            state = IN_STOP_BITS;
        } else {
            state = IN_PARITY_BIT;
        }
    }
}

void RMTUARTBitStreamer::streamParityBit(uint16_t &level, uint16_t &duration) {
    switch (parity) {
        case PARITY_EVEN:
            level = parityValue;
            break;
        case PARITY_ODD:
            level = parityValue ^ 0x0001;
            break;
        case PARITY_SPACE:
            level = 0;
            break;
        case PARITY_MARK:
            level = 1;
            break;
        case PARITY_NONE:
        default:
            fatalError("Tried to stream RMT UART parity in bad state");
    }
    duration = bitDuration;

    state = IN_STOP_BITS;
}

bool RMTUARTBitStreamer::streamStopBits(uint16_t &level, uint16_t &duration) {
    level = 1;
    duration = stopBitDuration;
    if (bytesRemaining > 0) {
        state = IN_START_BIT;
        return true;
    } else {
        state = COMPLETED;
        return false;
    }
}

bool RMTUARTBitStreamer::midStream() {
    return (state == IN_START_BIT) || (state == IN_CHARACTER_BITS) || (state == IN_PARITY_BIT) ||
               (state == IN_STOP_BITS);
}
