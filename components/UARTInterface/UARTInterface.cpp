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

#include "UARTInterface.h"
#include "Interface.h"

#include "Error.h"
#include "ESPError.h"
#include "Logger.h"

#include "etl/algorithm.h"

#include "driver/uart.h"
#include "esp_err.h"

#include <stddef.h>
#include <string.h>

UARTInterface::UARTInterface(const char *name, enum InterfaceProtocol protocol,
                             DataModelNode &protocolNode, uart_port_t uartNumber, int rxPin,
                             int txPin, int baudRate, size_t rxBufferSize, size_t stackSize)
    : Interface(name, protocol, protocolNode, stackSize),
      _uartNumber(uartNumber),
      rxPin(rxPin),
      txPin(txPin),
      baudRate(baudRate),
      rxBufferSize(rxBufferSize) {
}

void UARTInterface::startUART() {
    logger << logDebugUART << "Configuring UART " << _uartNumber << " for " << baudRate
           << " baud, rx pin " << rxPin << " tx pin " << txPin << eol;

    esp_err_t error;
    error = uart_driver_install(_uartNumber, rxBufferSize, 0, 0, nullptr, 0);
    if (error != ESP_OK) {
        logger << logErrorUART << "Failed to install UART " << _uartNumber << "  driver: "
               
               << ESPError(error) << eol;
        errorExit();
    }

    uart_config_t uart_config = {
        .baud_rate = baudRate,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 1,
        .source_clk = UART_SCLK_DEFAULT,
    };
    error = uart_param_config(_uartNumber, &uart_config);
    if (error != ESP_OK) {
        logger << logErrorUART << "Failed to configure UART " << _uartNumber << "  driver: "
               << ESPError(error) << eol;
        errorExit();
    }

    error = uart_set_pin(_uartNumber, txPin, rxPin, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    if (error != ESP_OK) {
        logger << logErrorUART << "Failed to assign UART " << _uartNumber << " pins: "
              << ESPError(error) << eol;
        errorExit();
    }

    error = uart_set_mode(_uartNumber, UART_MODE_UART);
    if (error != ESP_OK) {
        logger << logErrorUART << "Failed to set UART " << _uartNumber << " mode: "
               << ESPError(error) << eol;
        errorExit();
    }

    error = uart_set_rx_timeout(_uartNumber, rxTimeoutInChars);
    if (error != ESP_OK) {
        logger << logErrorUART << "Failed to set UART " << _uartNumber << " RX timeout: "
               << ESPError(error) << eol;
        errorExit();
    }
}

uart_port_t UARTInterface::uartNumber() const {
    return _uartNumber;
}

size_t UARTInterface::readToBuffer(void *buffer, size_t rxBufferSize) {
    size_t bytesInRxBuffer;
    esp_err_t error = uart_get_buffered_data_len(_uartNumber, &bytesInRxBuffer);
    if (error != ESP_OK) {
        logger << logErrorUART << "Failed to get UART " << _uartNumber
                << " RX character count: " << ESPError(error) << eol;
        errorExit();
    }

    if (bytesInRxBuffer) {
        size_t bytesToRead = etl::min(bytesInRxBuffer, rxBufferSize);
        size_t bytesRead = uart_read_bytes(_uartNumber, buffer, bytesToRead, 0);
        logger << logDebugUART << "Reading from UART " << _uartNumber << ", expected "
                << bytesInRxBuffer << " received " << bytesRead << eol;
        return bytesRead;
    } else {
        return 0;
    }
}

void UARTInterface::send(const char *string) {
    size_t length = strlen(string);
    logger << logDebugUART << "Writing " << length << " bytes to UART " << _uartNumber << eol;
    uart_write_bytes(_uartNumber, string, length);
}
