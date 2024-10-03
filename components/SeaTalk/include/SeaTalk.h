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

#ifndef SEA_TALK_H
#define SEA_TALK_H

#include <stddef.h>
#include <stdint.h>

#include "InterfaceParams.h"

static constexpr uint32_t SEA_TALK_BAUD_RATE = 4800;
static constexpr InterfaceDataWidth SEA_TALK_DATA_WIDTH = DATA_WIDTH_9_BITS;
static constexpr InterfaceParity SEA_TALK_PARITY = PARITY_NONE;
static constexpr InterfaceStopBits SEA_TALK_STOP_BITS = STOP_BITS_1;

static constexpr size_t MAX_SEA_TALK_LINE_LENGTH = 20; // Per Knauf: 18 is sufficient...

#endif // SEA_TALK_H
