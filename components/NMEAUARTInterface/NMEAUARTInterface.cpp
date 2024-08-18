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

#include "NMEAUARTInterface.h"
#include "UARTInterface.h"
#include "InterfaceProtocol.h"
#include "NMEASource.h"

#include "driver/uart.h"

#include <stdint.h>

NMEAUARTInterface::NMEAUARTInterface(const char *name, uart_port_t uartNumber, int rxPin, int txPin,
                                     int baudRate, StatsManager &statsManager,
                                     AISContacts &aisContacts, DataModel &dataModel)
    : UARTInterface(name, INTERFACE_NMEA_O183, uartNumber, rxPin, txPin, baudRate, rxBufferSize,
                    dataModel, stackSize),
      NMEASource(interfaceNode(), aisContacts, statsManager) {
}

void NMEAUARTInterface::task() {
    sourceReset();
    startUART();

    logger << logDebugNMEAUART << "Starting receive on UART " << uartNumber() << "..." << eol;

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
