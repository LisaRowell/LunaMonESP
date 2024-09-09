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

#include "NMEASoftUARTInterface.h"
#include "SoftUARTInterface.h"
#include "NMEAInterface.h"
#include "InterfaceProtocol.h"

#include "Logger.h"

#include "driver/uart.h"
#include "driver/gpio.h"

#include <stddef.h>
#include <stdint.h>

NMEASoftUARTInterface::NMEASoftUARTInterface(const char *name, const char *label, gpio_num_t rxPin,
                                             gpio_num_t txPin, uint32_t baudRate,
                                             StatsManager &statsManager, AISContacts &aisContacts,
                                             DataModel &dataModel)
    : SoftUARTInterface(name, label, INTERFACE_NMEA_O183, rxPin, txPin, baudRate, UART_DATA_8_BITS,
                        UART_STOP_BITS_1, UART_PARITY_DISABLE, statsManager, dataModel, stackSize),
      NMEAInterface(interfaceNode(), aisContacts, statsManager) {
}

void NMEASoftUARTInterface::task() {
    logger << logDebugNMEASoftUART << "Starting receive on Software UART " << name() << "..."
           << eol;

    startInterface();

    while (true) {
        // Currently we read by polling to see if there are characters in the UART's RX buffer,
        // reading them if there are, sleeping if there are not. A better implementation would be
        // to use an interrupt to wake the task...
        size_t bytesRead = readToByteBuffer((uint8_t *)buffer, rxBufferSize - 1);
        if (bytesRead) {
            processBuffer(buffer, bytesRead);
        } else {
            vTaskDelay(pdMS_TO_TICKS(noDataDelayMs));
        }
    }
}
