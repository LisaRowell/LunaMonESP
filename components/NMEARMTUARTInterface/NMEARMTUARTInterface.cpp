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

#include "NMEARMTUARTInterface.h"
#include "RMTUARTInterface.h"
#include "InterfaceProtocol.h"
#include "InterfaceMode.h"
#include "InterfaceParams.h"
#include "NMEAInterface.h"

#include "driver/gpio.h"

#include <stdint.h>

NMEARMTUARTInterface::NMEARMTUARTInterface(const char *name, const char *label, gpio_num_t rxGPIO,
                                           gpio_num_t txGPIO, uint32_t baudRate,
                                           const char *filteredTalkersList,
                                           StatsManager &statsManager, AISContacts &aisContacts,
                                           DataModel &dataModel)
    : RMTUARTInterface(name, label, INTERFACE_NMEA_O183, RX_AND_TX, baudRate, DATA_WIDTH_8_BITS,
                       PARITY_NONE, STOP_BITS_1, rxGPIO, txGPIO, rxBufferSize, statsManager,
                       dataModel, stackSize),
      NMEAInterface(interfaceNode(), filteredTalkersList, aisContacts, statsManager) {
}

void NMEARMTUARTInterface::task() {
    sourceReset();
    startUART();

    logger << logDebugNMEARMTUART << "Starting receive on RMT UART ..." << eol;

    while (true) {
        // Currently we read by polling to see if there are characters in the UART's RX buffer,
        // reading them if there are, sleeping if there are not. A better implementation would be
        // to use an interrupt to wake the task...
        size_t bytesRead = readToBuffer(buffer, rxBufferSize);
        if (bytesRead) {
            processBuffer(buffer, bytesRead);
        } else {
            vTaskDelay(pdMS_TO_TICKS(noDataDelayMs));
        }
    }
}
