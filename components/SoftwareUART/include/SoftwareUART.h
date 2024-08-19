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

#ifndef SOFTWARE_UART_H
#define SOFTWARE_UART_H

#include "TaskObject.h"

#include "driver/uart.h"
#include "driver/gpio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/stream_buffer.h"

#include <stdint.h>
#include <stddef.h>

class SoftwareUART :public TaskObject {
    private:
        typedef enum {
            SYNCHRONIZING,
            START_OF_FRAME,
            IN_STOP_BITS,
            MID_FRAME
        } ReceiveTaskState;

        static constexpr size_t bitTimeStreamEntries = 256;
        static constexpr size_t bitTimeStreamSize = (bitTimeStreamEntries + 1) * sizeof(uint32_t);
        static constexpr size_t receiveStreamEntries = 256;
        static constexpr size_t receiveStackSize = 1024;
        static constexpr uint32_t microSecondsPerSecond = 1000000;
        static constexpr size_t bitsInByte = 8;
        static constexpr size_t stackSize = (1024 * 8);

        gpio_num_t rxPin;
        gpio_num_t txPin;
        uint32_t microSecondsPerBit;
        uint8_t dataBitsPerFrame;
        uint8_t parityBits;
        uint32_t stopBitInterval;
        uint8_t bitsPerFrame;
        uint32_t frameInterval;
        uint8_t bytesPerFrame;

        StreamBufferHandle_t bitTimeStream;
        uint64_t lastBitTime;
        uint32_t nextRXLevel;
        bool receiveBitTimeOverrun;

        StreamBufferHandle_t receiveStream;
        bool receiveStreamOverrun;

        TaskHandle_t receiveTaskHandle;
        uint32_t rxBits;
        bool firstBit;
        uint32_t lastBit;
        ReceiveTaskState receiveTaskState;
        uint16_t frame;
        uint8_t bitsRemainingInFrame;
        bool frameComplete;

        void configure(uint32_t baudRate, uart_word_length_t dataBits, uart_stop_bits_t stopBits,
                       uart_parity_t parity);
        void task();
        void bitChangeWhileSynchronizing(uint32_t inputValue, uint32_t bitTime);
        void bitChangeAtStartOfFrame(uint32_t inputValue, uint32_t bitTime);
        void bitChangeMidFrame(uint32_t inputValue, uint32_t bitTime);
        void bitChangeInStopBits(uint32_t inputValue, uint32_t bitTime);
        void addBitsToFrame(uint32_t value, uint8_t count);
        void finishFrame();
        bool checkParity();
        void resetFrame();
        void configureGPIOs();

    public:
        SoftwareUART(gpio_num_t rxPin, gpio_num_t txPin, uint32_t baudRate,
                     uart_word_length_t dataBits, uart_stop_bits_t stopBits, uart_parity_t parity);
        size_t readToByteBuffer(uint8_t *buffer, size_t rxBufferSize);
        size_t readToShortBuffer(uint16_t *buffer, size_t rxBufferSize);
        uint32_t rxBitCount() const;
        bool receiveOverrunOccured() const;

        friend void IRAM_ATTR receiveISRHandler(void *arg);
};

#endif // SOFTWARE_UART_H
