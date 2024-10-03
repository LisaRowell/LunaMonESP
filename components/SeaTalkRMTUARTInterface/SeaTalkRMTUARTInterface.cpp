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

#include "SeaTalkRMTUARTInterface.h"
#include "SeaTalk.h"

#include "InterfaceProtocol.h"

#include "driver/gpio.h"

#include <stddef.h>
#include <stdint.h>

SeaTalkRMTUARTInterface::SeaTalkRMTUARTInterface(const char *name, const char *label,
                                                 gpio_num_t rxGPIO, gpio_num_t txGPIO,
                                                 InstrumentData &instrumentData,
                                                 StatsManager &statsManager, DataModel &dataModel)
    : RMTUARTInterface(name, label, INTERFACE_SEA_TALK, RX_AND_TX, SEA_TALK_BAUD_RATE,
                       SEA_TALK_DATA_WIDTH, SEA_TALK_PARITY, SEA_TALK_STOP_BITS, rxGPIO, txGPIO,
                       rxBufferSize * 2, statsManager, dataModel, stackSize),
      SeaTalkInterface(*this, instrumentData, statsManager) {
}

void SeaTalkRMTUARTInterface::task() {
    startUART();
    SeaTalkInterface::start();

    logger << logDebugSeaTalkRMTUART << "Starting receive on RMT UART..." << eol;

    while (true) {
        uint16_t buffer[rxBufferSize];
        size_t bytesRead = readToBuffer(buffer, rxBufferSize);
        if (bytesRead) {
            processBuffer(buffer, bytesRead);
        } else {
            vTaskDelay(pdMS_TO_TICKS(noDataDelayMs));
        }
    }
}
