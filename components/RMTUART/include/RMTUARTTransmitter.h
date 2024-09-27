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

#ifndef RMT_UART_TRANSMITTER_H
#define RMT_UART_TRANSMITTER_H

#include "RMTUARTBitStreamer.h"

#include "InterfaceParams.h"

#include "driver/gpio.h"
#include "driver/rmt_types.h"
#include "driver/rmt_tx.h"
#include "driver/rmt_encoder.h"

#include <stddef.h>
#include <stdint.h>

class RMTUARTTransmitter {
    private:
        static constexpr int interruptPriority = 1;
        static constexpr uint32_t resolutionHz = 1000000;

        gpio_num_t gpio;
        rmt_channel_handle_t channelHandle;
        rmt_transmit_config_t transmitConfig;
        rmt_encoder_handle_t encoderHandle;
        RMTUARTBitStreamer bitStreamer;

        void createChannel();
        void buildTransmitConfig();
        void createEncoder();

    public:
        RMTUARTTransmitter(uint32_t baudRate, InterfaceDataWidth dataWidth, InterfaceParity parity,
                           InterfaceStopBits stopBits, gpio_num_t gpio);
        void send(const void *characters, size_t length);
};

#endif // RMT_UART_TRANSMITTER_H
