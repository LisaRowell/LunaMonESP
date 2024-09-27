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

#include "RMTUARTTransmitter.h"
#include "RMTUARTBitStreamer.h"
#include "RMTSymbolWriter.h"

#include "InterfaceParams.h"

#include "Logger.h"
#include "ESPError.h"
#include "Error.h"

#include "driver/gpio.h"
#include "driver/rmt_types.h"
#include "driver/rmt_tx.h"
#include "driver/rmt_encoder.h"

#include <stddef.h>
#include <stdint.h>

RMTUARTTransmitter::RMTUARTTransmitter(uint32_t baudRate, InterfaceDataWidth dataWidth,
                                       InterfaceParity parity, InterfaceStopBits stopBits,
                                       gpio_num_t gpio)
    : gpio(gpio),
      bitStreamer(baudRate, dataWidth, parity, stopBits, resolutionHz) {
    logger() << logDebugRMTUART << "Initializing RMT UART transmitting on GPIO " << gpio << " at "
             << baudRate << " baud " << dataWidth << " " << parity << " " << stopBits << eol;

    createChannel();

    esp_err_t error;
    if ((error = rmt_enable(channelHandle)) != ESP_OK) {
        logger() << logErrorRMTUART << "Failed to enable tranmit RMT: " << ESPError(error) << eol;
        errorExit();
    }

    createEncoder();
    buildTransmitConfig();
}

void RMTUARTTransmitter::createChannel() {
    rmt_tx_channel_config_t channelConfig = {
        .gpio_num = gpio,
        .clk_src = RMT_CLK_SRC_DEFAULT,
        .resolution_hz = resolutionHz,
        // The below should actually be chip version specific if it's true that the ESP32S3 has 48
        // symbol blocks instead of 64.
        .mem_block_symbols = 64,
        .trans_queue_depth = 1,
        .intr_priority = interruptPriority,
        .flags = {
            .invert_out = false,
            .with_dma = false,
            .io_loop_back = false,
            .io_od_mode = false
        }
    };
    esp_err_t error;
    if ((error = rmt_new_tx_channel(&channelConfig, &channelHandle)) != ESP_OK) {
        logger() << logErrorRMTUART << "Failed to allocate transmit RMT channel: "
                 << ESPError(error) << eol;
        errorExit();
    }
}

void RMTUARTTransmitter::buildTransmitConfig() {
    rmt_transmit_config_t transmitConfig = {
        // The ESP32 documentation for loop_count is pretty vague, but from looking at the code a
        // count of 0 means to send once and only once.
        .loop_count = 0,
        .flags {
            .eot_level = 1,
            .queue_nonblocking = false
        }
    };
    this->transmitConfig = transmitConfig;
}

size_t rmtUARTEncoder(const void *data, size_t dataSize, size_t symbolsWritten, size_t symbolsFree,
                      rmt_symbol_word_t *symbols, bool *done, void *arg) {
    RMTUARTBitStreamer *bitStreamer = (RMTUARTBitStreamer *)arg;
    RMTSymbolWriter symbolWriter(symbols, symbolsFree);

    if (!bitStreamer->midStream()) {
        if (dataSize == 0) {
            *done = true;
            return 0;
        }
        bitStreamer->start(data, dataSize);
    }

    bool more;
    bool full;
    do {
        uint16_t level;
        uint16_t duration;
        more = bitStreamer->nextBit(level, duration);
        full = symbolWriter.addLevelDuration(level, duration);
    } while (more && !full);
    if (!more) {
        symbolWriter.finish();
    }
    *done = !more;
    return symbolWriter.written();
}

void RMTUARTTransmitter::createEncoder() {
    rmt_simple_encoder_config_t encoderConfig = {
        .callback = rmtUARTEncoder,
        .arg = &bitStreamer,
        // Really this just needs to be one as we fill in whatever space we have
        .min_chunk_size = 4
    };
    esp_err_t error;
    if ((error = rmt_new_simple_encoder(&encoderConfig, &encoderHandle)) != ESP_OK) {
        logger() << logErrorRMTUART << "Failed to create transmit RMT encoder: " << ESPError(error)
                 << eol;
        errorExit();
    }
}

void RMTUARTTransmitter::send(const void *characters, size_t length) {
    if (length == 0) {
        logger() << logDebugRMTUART << "Ignoring RMT UART send of zero bytes" << eol;
        return;
    }

    logger() << logDebugRMTUART << "Starting RMT UART transfer of " << length << " bytes from "
             << (uint32_t)characters << eol;

    esp_err_t error;
    if ((error = rmt_transmit(channelHandle, encoderHandle, characters, length,
                              &transmitConfig)) != ESP_OK) {
        logger() << logWarnRMTUART << "RMT transmit failed: " << ESPError(error) << eol;
        return;
    }

    // We're currently sending inplace, with the interrupt level encoding working directly off of
    // the passed in character array. If we return while the interrupt is running, we'll run the
    // risk of the data going out of scope and becoming invalid. The solution, for now, is to wait
    // until all transmissions have been completed, but alternatively we could copy the data, start
    // the transmission, and return.
    if ((error = rmt_tx_wait_all_done(channelHandle, -1)) != ESP_OK) {
        logger() << logWarnRMTUART << "RMT wait until transmit done failed: " << ESPError(error)
                 << eol;
    }
}
