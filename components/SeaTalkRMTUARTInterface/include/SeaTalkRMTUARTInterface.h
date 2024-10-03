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

#ifndef SEA_TALK_RMT_UART_INTERFACE_H
#define SEA_TALK_RMT_UART_INTERFACE_H

#include "RMTUARTInterface.h"
#include "SeaTalkInterface.h"
#include "SeaTalk.h"

#include "driver/gpio.h"

#include <stddef.h>
#include <stdint.h>

class InstrumentData;
class StatsManager;
class DataModel;

class SeaTalkRMTUARTInterface : public RMTUARTInterface, SeaTalkInterface {
    private:
        static constexpr size_t stackSize = (1024 * 8);
        static constexpr uint32_t noDataDelayMs = 20;
        static constexpr uint16_t rxBufferSize = MAX_SEA_TALK_LINE_LENGTH;

        virtual void task() override;

    public:
        SeaTalkRMTUARTInterface(const char *name, const char *label, gpio_num_t rxGPIO,
                                gpio_num_t txGPIO, InstrumentData &instrumentData,
                                StatsManager &statsManager, DataModel &dataModel);
};

#endif // SEA_TALK_RMT_UART_INTERFACE_H
