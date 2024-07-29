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

#include "NMEASockSource.h"
#include "NMEASource.h"

#include "AISContact.h"
#include "DataModelHundredthsUInt32Leaf.h"
#include "StatsManager.h"
#include "Logger.h"

#include <stddef.h>
#include <sys/socket.h>

NMEASockSource::NMEASockSource(AISContacts &aisContacts, DataModelUInt32Leaf &messagesLeaf,
                                DataModelUInt32Leaf &messageRateLeaf, StatsManager &statsManager)
    : NMEASource(aisContacts, messagesLeaf, messageRateLeaf, statsManager) {
}

void NMEASockSource::processNMEAStream(int sock) {
    sourceReset();

    while (true) {
        char buffer[maxNMEALineLength];
        ssize_t count = recv(sock, buffer, maxNMEALineLength, 0);
        if (count == 0) {
            logger() << logWarnNMEA << "NMEA source closed connection." << eol;
            return;
        } else if (count < 0) {
            logger() << logWarnNMEA << "NMEA source read failed:" << strerror(errno) << "("
                     << errno << ")" << eol;
            return;
        } else {
            processBuffer(buffer, count);
        }
    }
}
