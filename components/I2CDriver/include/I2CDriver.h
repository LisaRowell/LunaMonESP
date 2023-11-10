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

#ifndef I2C_DRIVER_H
#define I2C_DRIVER_H

#include <stdint.h>
#include <stddef.h>

class I2CMaster;

class I2CDriver {
    private:
        I2CMaster &ic2Master;
        uint8_t deviceAddr;

    protected:
        uint8_t readRegister(uint8_t offset);
        void writeRegister(uint8_t offset, uint8_t value);
        void readRegisters(uint8_t offset, uint8_t *registers, size_t count);

    public:
        I2CDriver(I2CMaster &ic2Master, uint8_t deviceAddr);
};

#endif //I2C_DRIVER_H
