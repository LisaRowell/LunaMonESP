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

#include "RMTUARTInterface.h"
#include "RMTUART.h"
#include "Interface.h"
#include "InterfaceProtocol.h"
#include "InterfaceMode.h"
#include "InterfaceParams.h"

#include "driver/gpio.h"
// for testing
#include "driver/uart.h"
#include "esp_log.h"

#include <stddef.h>

RMTUARTInterface::RMTUARTInterface(const char *name, InterfaceProtocol protocol, InterfaceMode mode,
                                   uint32_t baudRate, InterfaceDataWidth dataWidth,
                                   InterfaceParity parity, InterfaceStopBits stopBits,
                                   gpio_num_t rxGPIO, gpio_num_t txGPIO, size_t rxBufferSize,
                                   StatsManager &statsManager, DataModel &dataModel,
                                   size_t stackSize)
    : Interface(name, protocol, statsManager, dataModel, stackSize),
      rmtUART(InterfaceMode::RX_ONLY, baudRate, dataWidth, parity, stopBits, rxGPIO, txGPIO,
              rxBufferSize) {
//    createTestSender(txGPIO, baudRate);
}

void RMTUARTInterface::startUART() {
    rmtUART.startUART();
}

size_t RMTUARTInterface::readToBuffer(void *buffer, size_t bufferSize) {
    return rmtUART.receive(buffer, bufferSize);
}

size_t RMTUARTInterface::sendBytes(const void *bytes, size_t length) {
    return length;
}

bool RMTUARTInterface::sendMessageBytes(const void *bytes, size_t length, bool blocking) {
    return true;
}

static void uartSender(void *arg) {
    const char *message = "Hello World!\r\n";
    while (1) {
        vTaskDelay(pdMS_TO_TICKS(5 * 1000));
        ESP_LOGI("RMTUART", "Sending...");
        uart_write_bytes(UART_NUM_1, message, strlen(message));
    }
}

void RMTUARTInterface::createTestSender(gpio_num_t gpio, uint32_t baudRate) {
    uart_config_t uart_config = {
        .baud_rate = (int)baudRate,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .rx_flow_ctrl_thresh = 1,
        .source_clk = UART_SCLK_DEFAULT
    };
    int intr_alloc_flags = 0;

#if CONFIG_UART_ISR_IN_IRAM
    intr_alloc_flags = ESP_INTR_FLAG_IRAM;
#endif

    ESP_ERROR_CHECK(uart_driver_install(UART_NUM_1, 1024 * 2, 0, 0, NULL, intr_alloc_flags));
    ESP_ERROR_CHECK(uart_param_config(UART_NUM_1, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM_1, gpio, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE,
                                 UART_PIN_NO_CHANGE));

    xTaskCreate(uartSender, "UART Sender", 8 * 1024, NULL, 10, NULL);
}