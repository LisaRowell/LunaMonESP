/*
 * This file is part of LunaMon (https://github.com/LisaRowell/LunaMon)
 * Copyright (C) 2021-2023 Lisa Rowell
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

#ifndef STRING_TOOLS_H
#define STRING_TOOLS_H

#include <etl/string_view.h>

#include <stdint.h>
#include <stddef.h>

extern bool extractUInt32FromStringView(const etl::string_view &stringView, size_t start,
                                        size_t length, uint32_t &value,
                                        uint32_t maxValue = 0xffffffff);
extern bool extractUInt16FromStringView(const etl::string_view &stringView, size_t start,
                                        size_t length, uint16_t &value, uint16_t maxValue = 0xffff);
extern bool extractUInt8FromStringView(const etl::string_view &stringView, size_t start,
                                       size_t length, uint8_t &value, uint8_t maxValue = 0xff);

#endif
