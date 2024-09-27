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

#ifndef RMT_UART_H
#define RMT_UART_H

#include "InterfaceMode.h"
#include "InterfaceParams.h"

#include "driver/gpio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include <stddef.h>
#include <stdint.h>

class RMTUARTReceiver;
class RMTUARTTransmitter;

class RMTUART {
    private:
        QueueHandle_t rxQueue;
        RMTUARTReceiver *receiver;
        RMTUARTTransmitter *transmitter;

        void initializeRX(uint32_t baudRate, InterfaceDataWidth dataWidth, InterfaceParity parity,
                          InterfaceStopBits stopBits, gpio_num_t gpio, size_t bufferSize,
                          QueueHandle_t rxQueue);
        void initializeTX(uint32_t baudRate, InterfaceDataWidth dataWidth, InterfaceParity parity,
                          InterfaceStopBits stopBits, gpio_num_t gpio);

    public:
        RMTUART(InterfaceMode mode, uint32_t baudRate, InterfaceDataWidth dataWidth,
                InterfaceParity parity, InterfaceStopBits stopBits, gpio_num_t rxGPIO,
                gpio_num_t txGPIO, size_t rxBufferSize);
        void startUART();
        size_t receive(void *buffer, size_t bufferSize);
        size_t send(const void *characters, size_t length);
};

#endif // RMT_UART_H
