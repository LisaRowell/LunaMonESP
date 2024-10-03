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

#include "InterfaceParams.h"

#include "Logger.h"
#include "Error.h"

#include <stddef.h>

size_t interfaceDataWidthBits(InterfaceDataWidth dataWidth) {
    switch (dataWidth) {
        case DATA_WIDTH_5_BITS:
            return 5;
        case DATA_WIDTH_6_BITS:
            return 6;
        case DATA_WIDTH_7_BITS:
            return 7;
        case DATA_WIDTH_8_BITS:
            return 8;
        case DATA_WIDTH_9_BITS:
            return 9;
        default:
            fatalError("Invalid interface data width bits");
    }
}

size_t sizeofDataWidthCharacter(InterfaceDataWidth dataWidth) {
    switch (dataWidth) {
        case DATA_WIDTH_5_BITS:
        case DATA_WIDTH_6_BITS:
        case DATA_WIDTH_7_BITS:
        case DATA_WIDTH_8_BITS:
            return sizeof(char);
        case DATA_WIDTH_9_BITS:
            return sizeof(uint16_t);
        default:
            fatalError("Invalid interface data width bits");
    }
}

Logger & operator << (Logger &logger, InterfaceDataWidth dataWidth) {
    logger << "Data width " << interfaceDataWidthBits(dataWidth);

    return logger;
}

size_t interfaceParityBits(InterfaceParity parity) {
    if (parity == PARITY_NONE) {
        return 0;
    } else {
        return 1;
    }
}

extern Logger & operator << (Logger &logger, InterfaceParity parity) {
    switch (parity) {
        case PARITY_NONE:
            logger << "No parity";
            break;
        case PARITY_EVEN:
            logger << "Even parity";
            break;
        case PARITY_ODD:
            logger << "Odd parity";
            break;
        case PARITY_SPACE:
            logger << "Space parity";
            break;
        case PARITY_MARK:
            logger << "Mark parity";
            break;
        default:
            fatalError("Invalid interface parity");
    }

    return logger;
}

uint8_t interfaceHalfStopBits(InterfaceStopBits stopBits) {
    switch (stopBits) {
        case STOP_BITS_1:
            return 2;
        case STOP_BITS_1_5:
            return 3;
        case STOP_BITS_2:
            return 4;
        default:
            fatalError("Invalid interface stop bits");
    }
}

extern Logger & operator << (Logger &logger, InterfaceStopBits stopBits) {
    switch (stopBits) {
        case STOP_BITS_1:
            logger << "1 stop bit";
            break;
        case STOP_BITS_1_5:
            logger << "1.5 stop bits";
            break;
        case STOP_BITS_2:
            logger << "2 stop bits";
            break;
        default:
            fatalError("Invalid interface stop bits");
    }

    return logger;
}
