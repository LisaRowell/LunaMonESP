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

#ifndef UART_INTERFACE_H
#define UART_INTERFACE_H

#include "Interface.h"

#include "driver/uart.h"

#include <stddef.h>

class StatsManager;
class DataModelNode;

class UARTInterface : public Interface {
    private:
        static constexpr uint8_t rxTimeoutInChars = 2;

        uart_port_t _uartNumber;
        int rxPin;
        int txPin;
        int baudRate;
        size_t rxBufferSize;
        size_t txBufferSize;

    public:
        UARTInterface(const char *name, const char *label, enum InterfaceProtocol protocol,
                      uart_port_t uartNumber, int rxPin, int txPin, int baudRate,
                      size_t rxBufferSize, size_t txBufferSize, StatsManager &statsManager,
                      DataModel &dataModel, size_t stackSize);
        void startUART();
        uart_port_t uartNumber() const;
        size_t readToBuffer(void *buffer, size_t rxBufferSize);
        virtual size_t sendBytes(const void *bytes, size_t length) override;
};

#endif // UART_INTERFACE_H

