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

#ifndef RMT_UART_RECEIVER_H
#define RMT_UART_RECEIVER_H

#include "TaskObject.h"
#include "RMTCharBuilder.h"
#include "InterfaceParams.h"

#include "driver/gpio.h"
#include "driver/rmt_types.h"
#include "driver/rmt_rx.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/message_buffer.h"

#include <stddef.h>
#include <stdint.h>

class RMTUARTReceiver : public TaskObject {
    private:
        static constexpr size_t stackSize = 3 * 1024;
        static constexpr int interruptPriority = 1;
        static constexpr uint32_t nanoSecondsPerSecond = 1000000000;
        static constexpr uint32_t resolutionHz = 1000000;
        static constexpr uint32_t glitchPercent = 10;

        RMTCharBuilder charBuilder;
        uint32_t baudRate;
        InterfaceDataWidth dataWidth;
        InterfaceParity parity;
        InterfaceStopBits stopBits;
        gpio_num_t gpio;
        size_t itemBufferSize;
        rmt_channel_handle_t channelHandle;
        rmt_receive_config_t receiveConfig;
        MessageBufferHandle_t eventQueue;

        virtual void task() override;
        void initializeRMT();
        void createChannel();
        void buildReceiveConfig();

    public:
        RMTUARTReceiver(uint32_t baudRate, InterfaceDataWidth dataWidth, InterfaceParity parity,
                        InterfaceStopBits stopBits, gpio_num_t gpio, size_t bufferSize,
                        QueueHandle_t outputQueue);
};

#endif // RMT_UART_RECEIVER_H
