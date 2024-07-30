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

#include "NMEAUARTSource.h"
#include "NMEA.h"
#include "NMEASource.h"

#include "StatsManager.h"
#include "AISContacts.h"
#include "DataModelNode.h"
#include "DataModelUInt32Leaf.h"
#include "Error.h"
#include "ESPError.h"
#include "Logger.h"

#include "etl/algorithm.h"

#include "driver/uart.h"
#include "esp_err.h"

#include <freertos/task.h>

#include <stddef.h>

NMEAUARTSource::NMEAUARTSource(const char *name, uart_port_t uartNumber, int rxPin, int txPin,
                               int rtsPin, int baudRate, StatsManager &statsManager, NMEA &nmea,
                               AISContacts &aisContacts)
    : TaskObject("NMEAUARTSource", LOGGER_LEVEL_DEBUG, stackSize),
      NMEASource(aisContacts, messagesLeaf, messageRateLeaf, statsManager),
      uartNumber(uartNumber),
      rxPin(rxPin),
      txPin(txPin),
      rtsPin(rtsPin),
      baudRate(baudRate),
      nmeaUARTNode(name, &nmea.nmeaNode()),
      messagesLeaf("messages", &nmeaUARTNode),
      messageRateLeaf("messageRate", &nmeaUARTNode) {
}

void NMEAUARTSource::task() {
    logger << logDebugNMEAUART << "Configuring UART " << uartNumber << " for " << baudRate
           << " baud, rx pin " << rxPin << " tx pin " << txPin << " RTS pin " << rtsPin << eol;

    sourceReset();

    esp_err_t error;
    error = uart_driver_install(uartNumber, rxBufferSize, 0, 0, nullptr, 0);
    if (error != ESP_OK) {
        logger << logErrorNMEAUART << "Failed to install UART " << uartNumber << "  driver: "
               << ESPError(error) << eol;
        errorExit();
    }

    uart_config_t uart_config = {
        .baud_rate = baudRate,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 122,
        .source_clk = UART_SCLK_DEFAULT,
    };
    error = uart_param_config(uartNumber, &uart_config);
    if (error != ESP_OK) {
        logger << logErrorNMEAUART << "Failed to configure UART " << uartNumber << "  driver: "
               << ESPError(error) << eol;
        errorExit();
    }

    error = uart_set_pin(uartNumber, txPin, rxPin, rtsPin, UART_PIN_NO_CHANGE);
    if (error != ESP_OK) {
        logger << logErrorNMEAUART << "Failed to assign UART " << uartNumber << " pins: "
               << ESPError(error) << eol;
        errorExit();
    }

    error = uart_set_mode(uartNumber, UART_MODE_RS485_HALF_DUPLEX);
    if (error != ESP_OK) {
        logger << logErrorNMEAUART << "Failed to set UART " << uartNumber << " mode: "
               << ESPError(error) << eol;
        errorExit();
    }

    error = uart_set_rx_timeout(uartNumber, 0);
    if (error != ESP_OK) {
        logger << logErrorNMEAUART << "Failed to set UART " << uartNumber << " RX timeout: "
               << ESPError(error) << eol;
        errorExit();
    }

    logger << logDebugNMEAUART << "Starting receive on UART " << uartNumber << "..." << eol;

    while (true) {
        // Currently we read by polling to see if there are characters in the UART's RC buffer,
        // reading them if there are, sleeping if there are not. A better implementation would be
        // to use an interrupt to wake the task...
        size_t bytesInRxBuffer;
        error = uart_get_buffered_data_len(uartNumber, &bytesInRxBuffer);
        if (error != ESP_OK) {
            logger << logErrorNMEAUART << "Failed to get UART " << uartNumber
                   << " RX character count: " << ESPError(error) << eol;
            errorExit();
        }

        if (bytesInRxBuffer) {
            size_t bytesToRead = etl::min(bytesInRxBuffer, rxBufferSize);
            char buffer[rxBufferSize];
            size_t bytesRead = uart_read_bytes(uartNumber, buffer, bytesToRead, 0);
            logger << logDebugNMEAUART << "Reading from UART " << uartNumber << ", expected "
                   << bytesInRxBuffer << " received " << bytesRead << eol;
            processBuffer(buffer, bytesRead);
        } else {
            vTaskDelay(pdMS_TO_TICKS(noDataDelayMs));
        }
    }
}
