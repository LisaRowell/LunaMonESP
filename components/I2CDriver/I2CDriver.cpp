/*
 * This file is part of LunaMon (https://github.com/LisaRowell/LunaMon)
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

#include "I2CDriver.h"

#include "I2CMaster.h"

#include <stdint.h>

I2CDriver::I2CDriver(I2CMaster &ic2Master, uint8_t deviceAddr)
    : ic2Master(ic2Master), deviceAddr(deviceAddr) {
}

uint8_t I2CDriver::readRegister(uint8_t offset) {
    return ic2Master.readByte(deviceAddr, offset);
}

void I2CDriver::writeRegister(uint8_t offset, uint8_t value) {
    ic2Master.writeByte(deviceAddr, offset, value);
}

void I2CDriver::readRegisters(uint8_t offset, uint8_t *registers, size_t count) {
    ic2Master.readBytes(deviceAddr, offset, registers, count);
}
