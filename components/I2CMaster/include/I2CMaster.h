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

#ifndef I2C_MASTER
#define I2C_MASTER

#include <stdint.h>
#include <stddef.h>

class I2CMaster {
    public:
        I2CMaster();
        uint8_t readByte(uint8_t deviceAddr, uint8_t byteAddr);
        void writeByte(uint8_t deviceAddr, uint8_t byteAddr, uint8_t value);
        void readBytes(uint8_t deviceAddr, uint8_t address, uint8_t *bytes, size_t count);
};

#endif // I2C_MASTER