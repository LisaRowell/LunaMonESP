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

#ifndef SOFT_UART_INTERFACE_H
#define SOFT_UART_INTERFACE_H

#include "Interface.h"
#include "InterfaceProtocol.h"
#include "SoftwareUART.h"

#include "driver/gpio.h"
#include "driver/uart.h"

#include <stdint.h>
#include <stdint.h>

class DataModel;

class SoftUARTInterface  : public Interface {
    private:
        SoftwareUART uart;

    public:
        SoftUARTInterface(const char *name, enum InterfaceProtocol protocol, gpio_num_t rxPin,
                          gpio_num_t txPin, uint32_t baudRate, uart_word_length_t dataBits,
                          uart_stop_bits_t stopBits, uart_parity_t parity, DataModel &dataModel,
                          size_t stackSize);
        void startInterface();
        size_t readToByteBuffer(uint8_t *buffer, size_t rxBufferSize);
        size_t readToShortBuffer(uint16_t *buffer, size_t rxBufferSize);
        uint32_t rxBitCount() const;
        bool receiveOverrunOccured() const;
};

#endif // SOFT_UART_INTERFACE_H
