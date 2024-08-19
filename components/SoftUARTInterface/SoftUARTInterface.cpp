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

#include "SoftUARTInterface.h"
#include "Interface.h"
#include "InterfaceProtocol.h"
#include "SoftwareUART.h"

#include "Logger.h"
#include "Error.h"
#include "ESPError.h"

#include "driver/uart.h"
#include "driver/gpio.h"

#include <stddef.h>
#include <stdint.h>

SoftUARTInterface::SoftUARTInterface(const char *name, enum InterfaceProtocol protocol,
                                     gpio_num_t rxPin, gpio_num_t txPin, uint32_t baudRate,
                                     uart_word_length_t dataBits, uart_stop_bits_t stopBits,
                                     uart_parity_t parity, DataModel &dataModel, size_t stackSize)
    : Interface(name, protocol, dataModel, stackSize),
      uart(rxPin, txPin, baudRate, dataBits, stopBits, parity) {
}

void SoftUARTInterface::startInterface() {
    uart.start();
}

size_t SoftUARTInterface::readToByteBuffer(uint8_t *buffer, size_t rxBufferSize) {
    return uart.readToByteBuffer(buffer, rxBufferSize);
}

size_t SoftUARTInterface::readToShortBuffer(uint16_t *buffer, size_t rxBufferSize) {
    return uart.readToShortBuffer(buffer, rxBufferSize);
}

uint32_t SoftUARTInterface::rxBitCount() const {
    return uart.rxBitCount();
}

bool SoftUARTInterface::receiveOverrunOccured() const {
    return uart.receiveOverrunOccured();
}
