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

#ifndef RMT_UART_INTERFACE_H
#define RMT_UART_INTERFACE_H

#include "Interface.h"
#include "RMTUART.h"
#include "InterfaceProtocol.h"
#include "InterfaceMode.h"
#include "InterfaceParams.h"

#include "driver/gpio.h"

#include <stddef.h>
#include <stdint.h>

class RMTUARTInterface : public Interface {
    private:
        RMTUART rmtUART;

        // Test code
        void createTestSender(gpio_num_t gpio, uint32_t baudRate);

    public:
        RMTUARTInterface(const char *name, InterfaceProtocol protocol, InterfaceMode mode,
                         uint32_t baudRate, InterfaceDataWidth dataWidth, InterfaceParity parity,
                         InterfaceStopBits stopBits, gpio_num_t rxGPIO, gpio_num_t txGPIO,
                         size_t rxBufferSize, StatsManager &statsManager, DataModel &dataModel,
                         size_t stackSize);
        void startUART();
        size_t readToBuffer(void *buffer, size_t bufferSize);
        virtual size_t sendBytes(const void *bytes, size_t length) override;
        virtual bool sendMessageBytes(const void *bytes, size_t length,
                                      bool blocking = true) override;
};

#endif // RMT_UART_INTERFACE_H
