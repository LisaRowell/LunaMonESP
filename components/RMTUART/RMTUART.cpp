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
#include "RMTUARTTransmitter.h"

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
    sizeofCharacter = sizeofDataWidthCharacter(dataWidth);
    if ((rxQueue = xQueueCreate(rxBufferSize, sizeofCharacter)) == nullptr) {
        logger() << logErrorRMTUART << "Failed to allocate RX queue" << eol;
        errorExit();
    }

    switch (mode) {
        case InterfaceMode::RX_ONLY:
            initializeRX(baudRate, dataWidth, parity, stopBits, rxGPIO, rxBufferSize, rxQueue);
            break;
        case InterfaceMode::TX_ONLY:
            initializeTX(baudRate, dataWidth, parity, stopBits, txGPIO);
            break;
        case InterfaceMode::RX_AND_TX:
            initializeRX(baudRate, dataWidth, parity, stopBits, rxGPIO, rxBufferSize, rxQueue);
            initializeTX(baudRate, dataWidth, parity, stopBits, txGPIO);
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

void RMTUART::initializeTX(uint32_t baudRate, InterfaceDataWidth dataWidth, InterfaceParity parity,
                           InterfaceStopBits stopBits, gpio_num_t gpio) {
    logger() << logDebugRMTUART << "Creating RMT UART Transmitter" << eol;

    transmitter = new RMTUARTTransmitter(baudRate, dataWidth, parity, stopBits, gpio);
    if (transmitter == nullptr) {
        logger() << logErrorRMTUART << "Failed to allocate RMT UART Transmitter" << eol;
        errorExit();
    }
}

size_t RMTUART::receive(void *buffer, size_t bufferSize) {
    if (receiver == nullptr) {
        logger() << logErrorRMTUART << "Tried to receive from a transmit only RMT UART interface"
                 << eol;
        errorExit();
    }

    size_t charactersRead = 0;
    if (sizeofCharacter == sizeof(char)) {
        uint8_t *bufferPos = (uint8_t *)buffer;
        while ((charactersRead < bufferSize) &&
               (xQueueReceive(rxQueue, bufferPos, 0) == pdTRUE)) {
            bufferPos++;
            charactersRead++;
        }
    } else {
        uint16_t *bufferPos = (uint16_t *)buffer;
        while ((charactersRead < bufferSize) &&
               (xQueueReceive(rxQueue, bufferPos, 0) == pdTRUE)) {
            bufferPos++;
            charactersRead++;
        }
    }

    return charactersRead;
}

size_t RMTUART::send(const void *characters, size_t length) {
    if (transmitter == nullptr) {
        logger() << logErrorRMTUART << "Tried to send to a receive only RMT UART interface" << eol;
        errorExit();
    }

    transmitter->send(characters, length);

    return length;
}
