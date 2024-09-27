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

#ifndef INTERFACE_PARAMS_H
#define INTERFACE_PARAMS_H

#include <stddef.h>
#include <stdint.h>

class Logger;

enum InterfaceDataWidth : uint8_t {
    DATA_WIDTH_5_BITS,
    DATA_WIDTH_6_BITS,
    DATA_WIDTH_7_BITS,
    DATA_WIDTH_8_BITS,
    DATA_WIDTH_9_BITS
};

extern size_t interfaceDataWidthBits(InterfaceDataWidth dataWidth);

extern Logger & operator << (Logger &logger, InterfaceDataWidth dataWidth);

enum InterfaceParity : uint8_t {
    PARITY_NONE,
    PARITY_EVEN,
    PARITY_ODD,
    PARITY_SPACE,
    PARITY_MARK
};

extern size_t interfaceParityBits(InterfaceParity parity);

extern Logger & operator << (Logger &logger, InterfaceParity parity);

enum InterfaceStopBits : uint8_t {
    STOP_BITS_1,
    STOP_BITS_1_5,
    STOP_BITS_2
};

extern uint8_t interfaceHalfStopBits(InterfaceStopBits stopBits);

extern Logger & operator << (Logger &logger, InterfaceStopBits stopBits);

#endif // INTERFACE_PARAMS_H
