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

#include "RMTUART.h"
#include "RMTUARTReceiver.h"
#include "RMTSymbolReader.h"
#include "RMTCharBuilder.h"
#include "InterfaceMode.h"
#include "InterfaceParams.h"

#include "Logger.h"
#include "Error.h"

#include "driver/gpio.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include <stddef.h>
#include <stdint.h>

RMTUART::RMTUART(InterfaceMode mode, uint32_t baudRate, InterfaceDataWidth dataWidth,
                 InterfaceParity parity, InterfaceStopBits stopBits, gpio_num_t rxGPIO,
                 gpio_num_t txGPIO, size_t rxBufferSize)
    : receiver(nullptr) {
    // For now we just make the queue be of 8-bit characters, later this will need to support 9-bit
    // as well. We also need a read queue size...
    if ((rxQueue = xQueueCreate(rxBufferSize, sizeof(char))) == nullptr) {
        logger() << logErrorRMTUART << "Failed to allocate RX queue" << eol;
        errorExit();
    }

    switch (mode) {
        case InterfaceMode::RX_ONLY:
            initializeRX(baudRate, dataWidth, parity, stopBits, rxGPIO, rxBufferSize, rxQueue);
            break;
        case InterfaceMode::TX_ONLY:
            initializeTX();
            break;
        case InterfaceMode::RX_AND_TX:
            initializeRX(baudRate, dataWidth, parity, stopBits, rxGPIO, rxBufferSize, rxQueue);
            initializeTX();
            break;
        default:
            logger() << logErrorRMTUART << "Bad interface mode " << (uint8_t)mode << eol;
            errorExit();
    }
}

void RMTUART::startUART() {
    if (receiver != nullptr) {
        receiver->start();
    }
}

void RMTUART::initializeRX(uint32_t baudRate, InterfaceDataWidth dataWidth, InterfaceParity parity,
                           InterfaceStopBits stopBits, gpio_num_t gpio, size_t bufferSize,
                           QueueHandle_t rxQueue) {
    logger() << logDebugRMTUART << "Creating RMT UART Receiver" << eol;

    receiver = new RMTUARTReceiver(baudRate, dataWidth, parity, stopBits, gpio, bufferSize,
                                   rxQueue);
    if (receiver == nullptr) {
        logger() << logErrorRMTUART << "Failed to allocate RMT UART Receiver" << eol;
        errorExit();
    }
}

void RMTUART::initializeTX() {
}

size_t RMTUART::receive(void *buffer, size_t bufferSize) {
    // We need to do work here for 9 bit support...
    uint8_t *bufferPos = (uint8_t *)buffer;
    size_t bytesRead = 0;
    while ((bytesRead < bufferSize) &&
           (xQueueReceive(rxQueue, bufferPos, 0) == pdTRUE)) {
        bufferPos++;
        bytesRead++;
    }

    return bytesRead;
}
