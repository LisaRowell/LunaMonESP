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

#include "NMEASource.h"
#include "NMEA.h"
#include "NMEAMessageHandler.h"

#include "DataModelNode.h"
#include "DataModelUInt32Leaf.h"

#include "StatsManager.h"

#include "Error.h"

NMEASource::NMEASource(const char *name, DataModelNode &interfaceNode, AISContacts &aisContacts,
                       StatsManager &statsManager)
    : parser(aisContacts),
      messageHandlers(),
      messagesLeaf("messages", &interfaceNode),
      messageRateLeaf("messageRate", &interfaceNode) {
    statsManager.addStatsHolder(*this);
}

void NMEASource::addMessageHandler(NMEAMessageHandler &messageHandler) {
    if (messageHandlers.full()) {
        fatalError("Too many message handles for NMEA source");
    }

    messageHandlers.push_back(&messageHandler);
}

void NMEASource::handleLine(NMEALine &inputLine) {
    NMEAMessage *nmeaMessage = parser.parseLine(inputLine);
    if (nmeaMessage != nullptr) {
        nmeaMessage->log();

        for (NMEAMessageHandler *messageHandler : messageHandlers) {
            messageHandler->processMessage(nmeaMessage);
        }

        messagesCounter++;

        // While we're done with the nmeaMessage, we don't do a free here
        // since it was allocated with a static buffer and placement new.
    }
}

void NMEASource::exportStats(uint32_t msElapsed) {
    messagesCounter.update(messagesLeaf, messageRateLeaf, msElapsed);
}
