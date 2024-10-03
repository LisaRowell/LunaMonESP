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

#include "STALKRMTUARTInterface.h"
#include "RMTUARTInterface.h"
#include "InterfaceProtocol.h"
#include "InterfaceMode.h"
#include "InterfaceParams.h"
#include "STALKInterface.h"
#include "SeaTalkLampIntensity.h"

#include "driver/gpio.h"

#include <stdint.h>

STALKRMTUARTInterface::STALKRMTUARTInterface(const char *name, const char *label, gpio_num_t rxPin,
                                             gpio_num_t txPin, uint32_t baudRate,
                                             InstrumentData &instrumentData,
                                             StatsManager &statsManager, DataModel &dataModel)
    : RMTUARTInterface(name, label, INTERFACE_STALK, RX_AND_TX, baudRate, DATA_WIDTH_8_BITS,
                       PARITY_NONE, STOP_BITS_1, rxPin, txPin, rxBufferSize, statsManager,
                       dataModel, stackSize),
      STALKInterface(*this, instrumentData, statsManager),
      firstDigitalYachtsWorkaroundSent(false) {
    digitalYachtsWorkaroundTimer.setSeconds(digitalYachtsStartTimeSec);
}

void STALKRMTUARTInterface::task() {
    sourceReset();
    startUART();
    SeaTalkInterface::start();

    logger << logDebugSTALKRMTUART << "Starting receive on RMT UART..." << eol;

    while (true) {
        // Currently we read by polling to see if there are characters in the UART's RC buffer,
        // reading them if there are, sleeping if there are not. A better implementation would be
        // to use an interrupt to wake the task...
        size_t bytesRead = readToBuffer(buffer, rxBufferSize);
        if (bytesRead) {
            NMEALineSource::processBuffer(buffer, bytesRead);

            // To get around bugs in Digitial Yachts' ST-NMEA (ISO) converters which prevented some
            // units from having configuration messages stored in NVRAM, we can reconfigure them on
            // the fly.
            if (CONFIG_LUNAMON_DIGITAL_YACHTS_STALK_WORKAROUND_ENABLED) {
                workAroundDigitalYachtsBugs();
            }
        } else {
            vTaskDelay(pdMS_TO_TICKS(noDataDelayMs));
        }
    }
}

// To get around bugs in Digitial Yachts' ST-NMEA (ISO) converters which prevented some units from
// having configuration messages stored in NVRAM, we can reconfigure them on the fly.
void STALKRMTUARTInterface::workAroundDigitalYachtsBugs() {
    if (lastMessageIllformed()) {
        // We use a passive timer to keep from spamming the d
        if (digitalYachtsWorkaroundTimer.expired()) {
            sendDigitalYachtsSTALKConfig();
            digitalYachtsWorkaroundTimer.setSeconds(digitalYachtsResendTimeSec);
        }
    }
}

void STALKRMTUARTInterface::sendDigitalYachtsSTALKConfig() {
    if (!firstDigitalYachtsWorkaroundSent) {
        // The Digital Yachts ST-NMEA device's serial input doesn't do a great job of syncing on the
        // first character it receives. Before sending the first config, try sending a blank line.
        send("\r\n");
        firstDigitalYachtsWorkaroundSent = 1;
    }
    send("$PDGY,STalk,On\r\n");
}
