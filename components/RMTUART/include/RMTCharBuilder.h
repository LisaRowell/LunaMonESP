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

#ifndef RMT_CHAR_BUILDER_H
#define RMT_CHAR_BUILDER_H

#include "InterfaceParams.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include <stdint.h>

class RMTCharBuilder {
    private:
        enum StreamState : uint8_t {
            WAITING_OUT_FIRST_BITS,
            SYCHRONIZING,
            START_OF_FRAME,
            MID_FRAME_EXPECTING_1,
            MID_FRAME_EXPECTING_0,
            WAITING_FOR_STOP_BITS,
            DISCARD_STREAM
        } ReceiveTaskState;

        uint8_t bitsPerChar;
        // dataBitsPerFrame includes parity if configured
        uint8_t dataBitsPerFrame;
        uint16_t bitDuration;
        uint16_t bitDurationSlop;
        InterfaceParity parity;
        uint16_t minStopBitDuration;

        QueueHandle_t outputQueue;

        StreamState state;
        uint8_t bitsAccumulated;
        uint16_t dataBits;

        uint32_t frameErrors;
        uint32_t glitchBits;
        uint32_t shortStopBits;
        uint32_t runOnFrames;
        uint32_t wrongBitErrors;
        uint32_t parityErrors;
        uint32_t overrunErrors;

        void bitsReceivedWhileSynchronizing(uint16_t duration, uint16_t level);
        void addStartOfFrameBits(uint16_t duration, uint16_t level);
        void addMidFrameBitsExpecting1(uint16_t duration, uint16_t level);
        void addMidFrameBitsExpecting0(uint16_t duration, uint16_t level);
        void stopBitsReceived(uint16_t duration, uint16_t level);
        void addOneBits(uint8_t count);
        void addZeroBits(uint8_t count);
        void dataBitsCompleted();
        bool inGoodParity();
        bool evenNumberDataBits();
        uint16_t durationToFullBits(uint16_t duration);

    public:
        RMTCharBuilder(InterfaceDataWidth dataWidth, InterfaceParity parity,
                       InterfaceStopBits stopBits, uint16_t bitDuration, QueueHandle_t outputQueue);
        void addBits(uint16_t duration, uint16_t level);
        void streamComplete();
};

#endif // RMT_CHAR_BUILDER_H
