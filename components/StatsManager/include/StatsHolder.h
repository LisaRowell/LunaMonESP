/*
 * This file is part of LunaMon (https://github.com/LisaRowell/LunaMonESP)
 * Copyright (C) 2021-2024 Lisa Rowell
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

#ifndef STATS_HOLDER_H
#define STATS_HOLDER_H

#include "etl/intrusive_links.h"

#include <stdint.h>

constexpr size_t statsHolderLinkID = 0;
typedef etl::bidirectional_link<statsHolderLinkID> statsHolderLink;

class StatsHolder : public statsHolderLink {
    public:
        virtual void exportStats(uint32_t msElapsed) = 0;
};

#endif // STATS_HOLDER_H
