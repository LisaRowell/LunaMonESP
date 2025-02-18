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

#ifndef NMEA_UART_INTERFACE_H
#define NMEA_UART_INTERFACE_H

#include "UARTInterface.h"
#include "NMEAInterface.h"
#include "InterfaceProtocol.h"
#include "NMEALine.h"

#include "driver/gpio.h"
#include "driver/uart.h"

#include <stddef.h>
#include <stdint.h>

class StatsManager;
class AISContacts;
class DataModel;

class NMEAUARTInterface : public UARTInterface, public NMEAInterface {
    private:
        static constexpr size_t stackSize = 3 * 1024;
        static constexpr size_t rxBufferSize = maxNMEALineLength * 3;
        static constexpr size_t txBufferSize = maxNMEALineLength * 3;
        static constexpr uint32_t noDataDelayMs = 20;

        char buffer[rxBufferSize];

        virtual void task() override;

    public:
        NMEAUARTInterface(const char *name, const char *label, uart_port_t uartNumber,
                          gpio_num_t rxPin, gpio_num_t txPin, uint32_t baudRate,
                          const char *filteredTalkersList, StatsManager &statsManager,
                          AISContacts &aisContacts, DataModel &dataModel);
};

#endif // NMEA_UART_INTERFACE_H
