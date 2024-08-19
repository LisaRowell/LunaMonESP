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

/*
 * While this code is a fresh implementation, and not a port, it owes the concept
 * of using an interrupt process to produce a stream of edge transition timestamps
 * that are queued to a background process that makes data steam bytes out of them
 * to an Arduino library, ESPSoftwareSerial. That library is licensed under LGPL 2.1
 * and credited to Peter Lerup and Dirk O. Kaar. Like many useful Arduino libraries,
 * it undoubtedly had contributions from others as well.
 * 
 * The code contained within is not Arduino based and would not be a good choice to
 * port in that direction. If you need an Arduino ethos based solution start with
 * the above mentioned code instead. This is IDF based, making direct calls to the
 * Espressif API and using FreeRTOS directly.
 */

#include "SoftwareUART.h"
#include "TaskObject.h"

#include "Logger.h"
#include "ESPError.h"
#include "Error.h"

#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_timer.h"

#include "etl/algorithm.h"

#include "freertos/FreeRTOS.h"
#include "freertos/stream_buffer.h"
#include "freertos/task.h"

#include <stddef.h>
#include <stdint.h>

// Todo: Add priority setting to TaskObject creation and make the Soft UART task higher priority
// to avoid interrupt level overruns.
SoftwareUART::SoftwareUART(gpio_num_t rxPin, gpio_num_t txPin, uint32_t baudRate,
                           uart_word_length_t dataBits, uart_stop_bits_t stopBits,
                           uart_parity_t parity)
    : TaskObject("Soft UART", LOGGER_LEVEL_DEBUG, stackSize),
      rxPin(rxPin),
      txPin(txPin),
      lastBitTime(0),
      nextRXLevel(0x00000000),
      receiveBitTimeOverrun(false),
      receiveStreamOverrun(false),
      rxBits(0),
      firstBit(true),
      receiveTaskState(SYNCHRONIZING) {
    if ((bitTimeStream = xStreamBufferCreate(bitTimeStreamSize, sizeof(uint32_t))) == nullptr) {
        fatalError("Failed to create Software UART bit time stream");
    }

    configure(baudRate, dataBits, stopBits, parity);

    if ((receiveStream = xStreamBufferCreate(receiveStreamEntries * bytesPerFrame,
                                             bytesPerFrame)) == nullptr) {
        fatalError("Failed to create Software UART receive stream");
    }
}

void SoftwareUART::configure(uint32_t baudRate, uart_word_length_t dataBits,
                             uart_stop_bits_t stopBits, uart_parity_t parity) {
    microSecondsPerBit = (microSecondsPerSecond + baudRate / 2) / baudRate;

    switch (dataBits) {
        case UART_DATA_5_BITS:
            dataBitsPerFrame = 5;
            bytesPerFrame = 1;
            break;
        case UART_DATA_6_BITS:
            dataBitsPerFrame = 6;
            bytesPerFrame = 1;
            break;
        case UART_DATA_7_BITS:
            dataBitsPerFrame = 7;
            bytesPerFrame = 1;
            break;
        case UART_DATA_8_BITS:
            dataBitsPerFrame = 8;
            bytesPerFrame = 1;
            break;
        case UART_DATA_BITS_MAX:
            dataBitsPerFrame = 9;
            bytesPerFrame = 2;
            break;
        default:
            logger << logErrorSoftUART << "Unsupported Soft UART word length " << dataBits << eol;
            errorExit();
    }

    switch (parity) {
        case UART_PARITY_DISABLE:
            parityBits = 0;
            break;
        default:
            logger << logErrorSoftUART << "Unsupported Soft UART parity configuration "
                   << parityBits << eol;
            errorExit();
    }

    switch (stopBits) {
        case UART_STOP_BITS_1:
            stopBitInterval = microSecondsPerBit;
            break;
        case UART_STOP_BITS_1_5:
            stopBitInterval = microSecondsPerBit + microSecondsPerBit / 2;
            break;
        case UART_STOP_BITS_2:
            stopBitInterval = microSecondsPerBit * 2;
            break;
        default:
            logger << logErrorSoftUART << "Unsupported Soft UART stop bits " << stopBits << eol;
            errorExit();
    }

    bitsPerFrame = dataBitsPerFrame + parityBits;
    frameInterval = (1 + bitsPerFrame) * microSecondsPerBit + stopBitInterval;
}

void SoftwareUART::task() {
    logger << logDebugSoftUART << "Configuring GPIOs..." << eol;
    logger << logDebugSoftUART << "Running on core " << xPortGetCoreID() << eol;

    configureGPIOs();

    logger << logDebugSoftUART << "Starting bit time stream receiving with microsec/bit of "
           << microSecondsPerBit << eol;

    while (true) {        
        uint32_t bitResult;
        // The following should read four bytes every time since we set up the threshold on the
        // stream to be sizeof(uint32_t).
        size_t bytesRead = xStreamBufferReceive(bitTimeStream, &bitResult, sizeof(uint32_t),
                                                portMAX_DELAY);
        if (bytesRead != sizeof(uint32_t)) {
            fatalError("Software UART receive task read failed");
        }
        rxBits++;

        uint32_t bitTime = bitResult & 0xfffffffe;
        uint32_t inputValue = bitResult & 0x00000001;

        if (firstBit) {
            firstBit = false;
        } else {
            if (lastBit == inputValue) {
                logger << logDebugSoftUART << "Non-flipped bit time of " << bitTime
                       << " microseconds; value = " << inputValue << eol;
                receiveTaskState = SYNCHRONIZING;
                continue;
            }
        }
        lastBit = inputValue;

        if (bitTime < microSecondsPerBit / 2) {
            logger << logDebugSoftUART << "Short bit interval " << bitTime
                   << " microseconds; value = " << inputValue << eol;
            receiveTaskState = SYNCHRONIZING;
            lastBit = inputValue;
            continue;
        }

        switch (receiveTaskState) {
            case SYNCHRONIZING:
                bitChangeWhileSynchronizing(inputValue, bitTime);
                break;
            case START_OF_FRAME:
                bitChangeAtStartOfFrame(inputValue, bitTime);
                break;
            case MID_FRAME:
                bitChangeMidFrame(inputValue, bitTime);
                break;
            case IN_STOP_BITS:
                bitChangeInStopBits(inputValue, bitTime);
                break;
        }
    }
}

void SoftwareUART::bitChangeWhileSynchronizing(uint32_t inputValue, uint32_t bitTime) {
    // This routine is run when a bit change arrives, but we haven't found a solid point where
    // we know for sure that we are looking at a start bit. Since the monitoring of the bit stream
    // could have started in the middle of another device sending a byte, we can't simply look for
    // the first zero and call it a start bit. Currently we look for a high single longer than
    // the frame interval, including start, stop, and parity bits.
    if ((inputValue == 0) && (bitTime > frameInterval)) {
        receiveTaskState = START_OF_FRAME;
        resetFrame();
    }
}

void SoftwareUART::bitChangeAtStartOfFrame(uint32_t inputValue, uint32_t bitTime) {
    uint32_t bitIntervals = (bitTime + microSecondsPerBit / 2) / microSecondsPerBit;

    if (inputValue != 1) {
        // This really should not happen as we should only be in this state if the last bit level
        // was low (either from a synchronization or waiting for a start bit).
        logger << logDebugSoftUART << "Zero bit when waiting for end of start bit" << eol;
        receiveTaskState = SYNCHRONIZING;
        return;
    }

    uint8_t bitIntervalsAfterStart = bitIntervals - 1;
    if (bitIntervalsAfterStart > dataBitsPerFrame) {
        // The input level was low for longer than a frame, which doesn't smell right.
        receiveTaskState = SYNCHRONIZING;
        // Add the equivelant of a parity error indicator?
        return;
    }

    if (bitIntervalsAfterStart) {
        addBitsToFrame(0, bitIntervalsAfterStart);
        if (frameComplete) {
            receiveTaskState = IN_STOP_BITS;
        } else {
            receiveTaskState = MID_FRAME;
        }
    } else{
        receiveTaskState = MID_FRAME;
    }    
}

void SoftwareUART::bitChangeMidFrame(uint32_t inputValue, uint32_t bitTime) {
    uint32_t bitIntervals = (bitTime + microSecondsPerBit / 2) / microSecondsPerBit;

    if (bitsRemainingInFrame > bitIntervals) {
        addBitsToFrame(inputValue ^ 1, bitIntervals);
    } else if (bitsRemainingInFrame == bitIntervals) {
        if (inputValue == 1) {
            // The last data or parity bit was a 0, so we're now in a stop bit
            addBitsToFrame(0, bitIntervals);
            receiveTaskState = IN_STOP_BITS;
        } else {
            // We ended the frame with a data or parity bit of 1, but that value didn't stay for
            // stop bits. Not a good look.
            logger << logDebugSoftUART << "Missing stop bits at end of frame" << eol;
            receiveTaskState = SYNCHRONIZING;
        }
    } else {
        if (inputValue == 0) {
            // We've finished the frame and had at least some time for stop bits. Make sure it was
            // enough.
            uint32_t stopBitTime = bitTime - microSecondsPerBit * bitsRemainingInFrame;
            if (stopBitTime > stopBitInterval - microSecondsPerBit / 2) {
                addBitsToFrame(1, bitsRemainingInFrame);
                finishFrame();
                receiveTaskState = START_OF_FRAME;
            } else {
                logger << logDebugSoftUART << "Short stop bits after frame ending in 1" << eol;
                receiveTaskState = SYNCHRONIZING;
            }
        } else {
            // We ended the frame with an elogated stretch of low values, past where a stop bit
            // should be.
            logger << logDebugSoftUART << "Frame ran into stop bits" << eol;
            receiveTaskState = SYNCHRONIZING;
        }
    }
}

void SoftwareUART::bitChangeInStopBits(uint32_t inputValue, uint32_t bitTime) {
    if (inputValue != 0) {
        // This really should not happen as we should only be in this state if the last bit
        // level was high (either from a synchronization or waiting for a start bit).
        logger << logDebugSoftUART << "One bit while already in a stop bit" << eol;
        receiveTaskState = SYNCHRONIZING;
        return;
    }

    if (bitTime + microSecondsPerBit / 2 > stopBitInterval) {
        finishFrame();
        receiveTaskState = START_OF_FRAME;
    } else {
        // We previously collected what we previously thought was a complete frame, but our stop
        // bits are messed up.
        logger << logDebugSoftUART << "Truncated stop bit interval of " << bitTime
               << " microseconds" << eol;
        receiveTaskState = SYNCHRONIZING;
    }
}

void SoftwareUART::addBitsToFrame(uint32_t value, uint8_t count) {
    if (count > bitsRemainingInFrame) {
        logger << logErrorSoftUART << "Attempt to add bits to a frame in excess of frame size. "
               << bitsRemainingInFrame << " were anticipated, " << count << " was added" << eol;
        errorExit();
    }
    if (value != 0 && value != 1) {
        logger << logErrorSoftUART << "Bad addBitsToFrame value " << value << eol;
        errorExit();
    }

    // Since the data stream comes with the least significant bit arriving first, be build the
    // frame by shifting in bits into the most significant bit. When the frame is complete, the
    // most significant bit will hold the parity bit, if configured, then the data bits, followed
    // by zeros in the lower bits. finishFrame() takes care of putting the bits in their proper
    // place.
    if (value) {
        frame = (frame >> count) | (0xffff << (16 - count)); 
    } else {
        frame = frame >> count; 
    }

    bitsRemainingInFrame -= count;
}

void SoftwareUART::finishFrame() {
    // We gotten all of the bits and have received the stop bit(s). Check parity and queue the
    // data word if indicated
    if (checkParity()) {
        frame <<= parityBits;               // Strip parity
        frame >>= 16 - bitsPerFrame;        // Get the bits in the normal position with MSBs of 0
        size_t bytesWritten;
        if (bytesPerFrame == 1) {
            uint8_t byteFrame = (uint8_t)frame;
            bytesWritten = xStreamBufferSend(receiveStream, &byteFrame, 1, 0);
        } else {
            bytesWritten = xStreamBufferSend(receiveStream, &frame, 2, 0);
        }
        if (bytesWritten != bytesPerFrame) {
            logger << logDebugSoftUART << "Receive stream overrun" << eol;
            receiveStreamOverrun = true;
        }
    }

    resetFrame();
}

bool SoftwareUART::checkParity() {
    //Currently unimplemented
    // When implementing, remember that the parity is in the most significant bit, followed by the
    // data bits, followed by zeros in the low order bit positions.
    return true;
}

void SoftwareUART::resetFrame() {
    bitsRemainingInFrame = bitsPerFrame;
    frameComplete = false;
    frame = 0x0000;
}

// The following interrupt handler alternates between looking for a low RX GPIO and a high one so
// as to detect bit level changes in the serial stream. We note the time of interrupt and enqueue
// it to the background process for interpretation and conversion into actual bytes.
//
// While it would be tempting to convert this to use edge triggered GPIO interrupts, testing has
// found that on at least some ESP32 variants, spurious interrupts and lost ones can be encountered,
// resulting in an input error rate of approximately 1 per 1000 bit changes. Since one of the
// intended uses of this component is SeaTalk communication, which lacks any form of partity or
// datagram level error detection, this error rate is not acceptable. By switching to level
// interrupts we seem to get around the issues.
void IRAM_ATTR receiveISRHandler(void *arg) {
    SoftwareUART *uart = (SoftwareUART *)arg;
    StreamBufferHandle_t bitTimeStream = uart->bitTimeStream;

    // This is buggy and will lose a but every way too many years. *shrug*
    uint64_t now = esp_timer_get_time();
    uint64_t timeSinceLastBit = now - uart->lastBitTime;
    uint32_t bitTime = timeSinceLastBit > UINT32_MAX ? UINT32_MAX : (uint32_t) timeSinceLastBit;
    uart->lastBitTime = now;

    uint32_t pinState;
    if (uart->nextRXLevel == 0x00000000) {
        pinState = 0x00000000;
        uart->nextRXLevel = 0x00000001;
        (void)gpio_set_intr_type(uart->rxPin, GPIO_INTR_HIGH_LEVEL);
    } else {
        pinState = 0x00000001;
        uart->nextRXLevel = 0x00000000;
        (void)gpio_set_intr_type(uart->rxPin, GPIO_INTR_LOW_LEVEL);
    }
    uint32_t bitResult = (bitTime & 0xfffffffe) | pinState;

    if (xStreamBufferSpacesAvailable(bitTimeStream) < sizeof(uint32_t)) {
        uart->receiveBitTimeOverrun = true;
    } else {
        (void)xStreamBufferSendFromISR(bitTimeStream, &bitResult, sizeof(uint32_t), nullptr);
    }
}

size_t SoftwareUART::readToByteBuffer(uint8_t *buffer, size_t rxBufferSize) {
    return xStreamBufferReceive(receiveStream, buffer, rxBufferSize, 0);
}

size_t SoftwareUART::readToShortBuffer(uint16_t *buffer, size_t rxBufferSize) {
    size_t bytesInStream = xStreamBufferBytesAvailable(receiveStream);
    size_t wordsInStream = bytesInStream / 2;
    size_t wordsToRead = etl::min(wordsInStream, rxBufferSize);
    size_t bytesToRead = wordsToRead * sizeof(uint16_t);

    return xStreamBufferReceive(receiveStream, buffer, bytesToRead, 0);
}

uint32_t SoftwareUART::rxBitCount() const {
    return rxBits;
}

bool SoftwareUART::receiveOverrunOccured() const {
    return receiveBitTimeOverrun;
}

void SoftwareUART::configureGPIOs() {
    esp_err_t error;

    gpio_config_t rxGPIOConfig = {
        .pin_bit_mask = 1ULL << rxPin,
        .mode = GPIO_MODE_INPUT,
        // We enable the pullup so that if the device connected isn't power up, or is disconnected
        // entirely, we don't get flooded with spurious interrupts. Consider making this
        // configurable for board designes where the pin would always be driven
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        // See the interrupt handler for a detail explanation of why a level interrupt is configured
        .intr_type = GPIO_INTR_LOW_LEVEL
    };

    if ((error = gpio_config(&rxGPIOConfig)) != ESP_OK) {
        logger << logErrorSoftUART << "Failed to configure RX pin " << rxPin << ": "
               << ESPError(error) << eol;
        errorExit();
    }

    if ((error = gpio_install_isr_service(ESP_INTR_FLAG_LEVEL3)) != ESP_OK) {
        logger << logErrorSoftUART << "Failed to install GPIO ISR service:" << ESPError(error)
               << eol;
        errorExit();
    }

    if ((error = gpio_set_level(rxPin, ESP_INTR_FLAG_LEVEL3)) != ESP_OK) {
        logger << logErrorSoftUART << "Failed to set interrupt level:" << ESPError(error)
               << eol;
        errorExit();
    }

    if ((error = gpio_isr_handler_add(rxPin, receiveISRHandler, this)) != ESP_OK) {
        logger << logErrorSoftUART << "Failed to register ISR Handler for RX pin " << rxPin
               << ": " << ESPError(error) << eol;
        errorExit();
    }

    if ((error = gpio_intr_enable(rxPin)) != ESP_OK) {
        logger << logErrorSoftUART << "Failed to enable interrupt for RX pin " << rxPin
               << ": " << ESPError(error) << eol;
        errorExit();
    }
}
