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

#include "RMTUARTInterface.h"
#include "RMTUART.h"
#include "Interface.h"
#include "InterfaceProtocol.h"
#include "InterfaceMode.h"
#include "InterfaceParams.h"

#include "driver/gpio.h"
#include "esp_log.h"

#include <stddef.h>

RMTUARTInterface::RMTUARTInterface(const char *name, const char *label, InterfaceProtocol protocol,
                                   InterfaceMode mode, uint32_t baudRate,
                                   InterfaceDataWidth dataWidth, InterfaceParity parity,
                                   InterfaceStopBits stopBits, gpio_num_t rxGPIO, gpio_num_t txGPIO,
                                   size_t rxBufferSize, StatsManager &statsManager,
                                   DataModel &dataModel, size_t stackSize)
    : Interface(name, label, protocol, statsManager, dataModel, stackSize),
      rmtUART(InterfaceMode::RX_AND_TX, baudRate, dataWidth, parity, stopBits, rxGPIO, txGPIO,
              rxBufferSize) {
}

void RMTUARTInterface::startUART() {
    rmtUART.startUART();
}

size_t RMTUARTInterface::readToBuffer(void *buffer, size_t bufferSize) {
    return rmtUART.receive(buffer, bufferSize);
}

size_t RMTUARTInterface::sendBytes(const void *bytes, size_t length) {
    return rmtUART.send(bytes, length);
}
