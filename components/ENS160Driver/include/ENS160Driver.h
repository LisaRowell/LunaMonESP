/*
 * This file is part of LunaMon (https://github.com/LisaRowell/LunaMonESP)
 * Copyright (C) 2023 Lisa Rowell
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

#ifndef ENS160_DRIVER_H
#define ENS160_DRIVER_H

#include "I2CDriver.h"

#include <stdint.h>

class I2CMaster;

class ENS160Driver : I2CDriver {
    private:
        void logStatus(uint8_t status);

    public:
        ENS160Driver(I2CMaster &ic2Master, uint8_t deviceAddr);
        bool detect();
        void start();
        bool read(bool &startingUp, uint8_t &aqi, uint16_t &tvoc, uint16_t &eco2);
        const char *aqiDescription(uint8_t aqi);
        const char *tvocDescription(uint16_t tvoc);
        const char *eco2Description(uint16_t eco2);
};

#endif // ENS160_DRIVER_H
