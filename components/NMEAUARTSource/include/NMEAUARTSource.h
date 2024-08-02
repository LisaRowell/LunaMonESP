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

#ifndef NMEA_UART_SOURCE_H
#define NMEA_UART_SOURCE_H

#include "TaskObject.h"
#include "NMEASource.h"

#include "DataModelNode.h"
#include "DataModelUInt32Leaf.h"

#include "driver/uart.h"

#include <stddef.h>
#include <stdint.h>

class StatsManager;
class NMEA;
class AISContacts;

class NMEAUARTSource : public TaskObject, public NMEASource {
    private:
        static constexpr size_t stackSize = (1024 * 8);
        static constexpr size_t rxBufferSize = maxNMEALineLength * 3;
        static constexpr uint32_t noDataDelayMs = 20;
        static constexpr uint8_t rxTimeoutInChars = 2;

        uart_port_t uartNumber;
        int rxPin;
        int txPin;
        int baudRate;
        DataModelNode nmeaUARTNode;
        DataModelUInt32Leaf messagesLeaf;
        DataModelUInt32Leaf messageRateLeaf;

        virtual void task() override;

    public:
        NMEAUARTSource(const char *name, uart_port_t uartNumber, int rxPin, int txPin, int baudRate,
                       StatsManager &statsManager, NMEA &nmea, AISContacts &aisContacts);
};

#endif // NMEA_UART_SOURCE_H
