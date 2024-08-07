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

#include "STALKSource.h"

#include "NMEALine.h"
#include "StatCounter.h"
#include "StatsManager.h"
#include "DataModelNode.h"
#include "DataModelUInt32Leaf.h"
#include "Logger.h"

#include <stdint.h>

STALKSource::STALKSource(const char *name, DataModelNode &interfaceNode,
                         StatsManager &statsManager)
    : messagesLeaf("messages", &interfaceNode),
      messageRateLeaf("messageRate", &interfaceNode) {
    statsManager.addStatsHolder(*this);
}

void STALKSource::handleLine(NMEALine &inputLine) {
    logger() << logDebugSTALK << inputLine << eol;
}

void STALKSource::exportStats(uint32_t msElapsed) {
    messagesCounter.update(messagesLeaf, messageRateLeaf, msElapsed);
}
