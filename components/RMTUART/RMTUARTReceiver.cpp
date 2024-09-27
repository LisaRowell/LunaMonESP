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

#include "RMTUARTReceiver.h"
#include "TaskObject.h"
#include "RMTSymbolReader.h"
#include "RMTCharBuilder.h"
#include "InterfaceParams.h"

#include "Logger.h"
#include "Error.h"
#include "ESPError.h"

#include "etl/algorithm.h"

#include "driver/gpio.h"
#include "driver/rmt_types.h"
#include "driver/rmt_rx.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include <stddef.h>
#include <stdint.h>

RMTUARTReceiver::RMTUARTReceiver(uint32_t baudRate, InterfaceDataWidth dataWidth,
                                 InterfaceParity parity, InterfaceStopBits stopBits,
                                 gpio_num_t gpio, size_t bufferSize, QueueHandle_t outputQueue)
    : TaskObject::TaskObject("RMT UART Receiver", LOGGER_LEVEL_DEBUG, stackSize,
                             TaskPriority::MEDIUM_PRIORITY),
      charBuilder(dataWidth, parity, stopBits, resolutionHz / baudRate, outputQueue),
      baudRate(baudRate),
      dataWidth(dataWidth),
      parity(parity),
      stopBits(stopBits),
      gpio(gpio) {
    itemBufferSize = bufferSize * 8;

    // We use FreeRTOS's Message Buffer mechanism to send event data from the interrupt callback to
    // the bit duration analysis task instead their Queue mechanism as it's lighter weight and we
    // don't require protection against multiple consumers or producers. We currently only have one
    // buffer so at most one message can be in flight. The size calculation includes a
    // sizeof(size_t) as this is the overhead per message internally in the message buffer.
    eventQueue = xMessageBufferCreate(1 * (sizeof(rmt_rx_done_event_data_t) + sizeof(size_t)));
    if (eventQueue == nullptr) {
        logger << logErrorRMTUART << "Failed to allocation receive event queue" << eol;
        errorExit();
    }
}

extern "C" bool rmtRXDoneCallback(rmt_channel_handle_t channel,
                                  const rmt_rx_done_event_data_t *eventData, void *userData) {
    MessageBufferHandle_t eventQueue = (MessageBufferHandle_t)userData;
    BaseType_t higherTaskWakeup = pdFALSE;
    (void)xMessageBufferSendFromISR(eventQueue, eventData, sizeof(rmt_rx_done_event_data_t),
                                    &higherTaskWakeup );

    return higherTaskWakeup == pdTRUE;
}

void RMTUARTReceiver::task() {
    initializeRMT();

    while (true != false) {
        rmt_symbol_word_t symbolWords[itemBufferSize / 2];

        esp_err_t error;
        if ((error = rmt_receive(channelHandle, symbolWords, sizeof(symbolWords),
                                 &receiveConfig)) != ESP_OK) {
            logger << logErrorRMTUART << "Call to rmt_receive failed: " << ESPError(error) << eol;
            errorExit();
        }

        // Wait for a completed RMT receive sent up from the interrupt level callback
        rmt_rx_done_event_data_t eventData;
        size_t eventDataRead; 
        if ((eventDataRead = xMessageBufferReceive(eventQueue, &eventData,
                                                   sizeof(rmt_rx_done_event_data_t),
                                                   portMAX_DELAY)) != 0) {
            if (eventDataRead != sizeof(rmt_rx_done_event_data_t)) {
                logger << logErrorRMTUART
                       << "Unexpected message read from receive queue. Expected a size of "
                       << sizeof(rmt_rx_done_event_data_t) << ", but got " << eventDataRead << eol;
                errorExit();
            }

            RMTSymbolReader symbolReader(eventData.received_symbols, eventData.num_symbols);
            uint16_t duration;
            uint16_t level;
            while (symbolReader.nextSymbol(duration, level)) {
                charBuilder.addBits(duration, level);
            }
            charBuilder.streamComplete();
        }
    }
}

void RMTUARTReceiver::initializeRMT() {
    logger << logDebugRMTUART << "Initializing RMT UART receiving on GPIO " << gpio << " at "
           << baudRate << " baud " << dataWidth << " " << parity << " " << stopBits << eol;

    createChannel();

    esp_err_t error;
    if ((error = rmt_enable(channelHandle)) != ESP_OK) {
        logger << logErrorRMTUART << "Failed to enable receive RMT: " << ESPError(error) << eol;
        errorExit();
    }

    // Build and cache a receive configuration structure to be used each time we initiate a receive
    buildReceiveConfig();
}

void RMTUARTReceiver::createChannel() {
    esp_err_t error;

    rmt_rx_channel_config_t channelConfig = {
        .gpio_num = gpio,
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = resolutionHz,
        .mem_block_symbols = itemBufferSize,
        .intr_priority = interruptPriority,
        .flags = {
            .invert_in = false,
            .with_dma = true,
            .io_loop_back = false,
        }
    };
    if ((error = rmt_new_rx_channel(&channelConfig, &channelHandle)) != ESP_OK) {
        logger << logErrorRMTUART << "Failed to allocate receive RMT channel: " << ESPError(error)
               << eol;
        errorExit();
    }

    rmt_rx_event_callbacks_t receiveCallbacks = {
        .on_recv_done = rmtRXDoneCallback,
    };
    if ((error = rmt_rx_register_event_callbacks(channelHandle, &receiveCallbacks,
                                                 eventQueue)) != ESP_OK) {
        logger << logErrorRMTUART << "Failed to register receive RMT callbacks: " << ESPError(error)
               << eol;
        errorExit();
    }
}

void RMTUARTReceiver::buildReceiveConfig() {
    // We build the structure used each receive call in order to save a couple of cycles
    uint32_t nanoSecondsPerBit = nanoSecondsPerSecond / baudRate;
    uint32_t frameTimeNS = nanoSecondsPerBit +
                           nanoSecondsPerBit * interfaceDataWidthBits(dataWidth) +
                           (nanoSecondsPerBit / 2) * interfaceHalfStopBits(stopBits); 

    // The signal_range_min_ns configuration parameter is designed to set a minimum pulse width
    // duration below which is considered noise. Handy as a UART signal is likely to have ringing on
    // level transitions. Ideally we would calulate this value solely as a percentage of bit,
    // duration, but there is an undocumented maximum allowed minimum duration. (at least in
    // ESP-IDF v5.3-dev-687-g48d732f49c). Below we calculate the filter duration we'd like based on
    // bit duration (which is based on baud rate), then force it to conform to the max based on
    // a reverse of the math in the IDF code.
    uint32_t minAcceptableBitDuration = nanoSecondsPerBit / glitchPercent;
    uint32_t maxConfigurableMinBitDuration = 1000000000UL / resolutionHz;
    uint32_t signalRangeMinNS = etl::min(minAcceptableBitDuration, maxConfigurableMinBitDuration);

    rmt_receive_config_t receiveConfig = {
        .signal_range_min_ns = signalRangeMinNS,
        .signal_range_max_ns = frameTimeNS + (nanoSecondsPerBit / glitchPercent),
        .flags = {
            .en_partial_rx = false
        }
    };
    this->receiveConfig = receiveConfig;
}
