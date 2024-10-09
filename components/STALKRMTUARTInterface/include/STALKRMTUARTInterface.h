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

#ifndef STALK_RMT_UART_INTERFACE_H
#define STALK_RMT_UART_INTERFACE_H

#include "RMTUARTInterface.h"
#include "InterfaceProtocol.h"
#include "STALKInterface.h"
#include "SeaTalkLampIntensity.h"
#include "NMEALine.h"
#include "PassiveTimer.h"

#include "driver/gpio.h"

#include <stddef.h>
#include <stdint.h>

class InstrumentData;
class StatsManager;
class DataModel;

class STALKRMTUARTInterface : public RMTUARTInterface, public STALKInterface {
    private:
        static constexpr size_t stackSize = (1024 * 8);
        static constexpr size_t rxBufferSize = maxNMEALineLength * 3;
        static constexpr uint32_t noDataDelayMs = 20;
        static constexpr uint32_t digitalYachtsStartTimeSec = 5;
        static constexpr uint32_t digitalYachtsResendTimeSec = 30;

        char buffer[rxBufferSize];
        PassiveTimer digitalYachtsWorkaroundTimer;
        bool firstDigitalYachtsWorkaroundSent;

        virtual void task() override;
        void workAroundDigitalYachtsBugs();
        void sendDigitalYachtsSTALKConfig();

    public:
        STALKRMTUARTInterface(const char *name, const char *label, gpio_num_t rxPin,
                              gpio_num_t txPin, uint32_t baudRate, InstrumentData &instrumentData,
                              StatsManager &statsManager, DataModel &dataModel);
};

#endif // STALK_RMT_UART_INTERFACE_H