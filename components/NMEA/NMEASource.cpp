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

#include "NMEASource.h"
#include "NMEALine.h"
#include "NMEAParser.h"
#include "NMEAMessage.h"
#include "NMEAMessageHandler.h"

#include "DataModelUInt32Leaf.h"

#include "StatsManager.h"

#include "StatCounter.h"

#include "CharacterTools.h"
#include "Logger.h"
#include "Error.h"

#include <stddef.h>
#include <string.h>
#include <sys/socket.h>

NMEASource::NMEASource(DataModelUInt32Leaf &messagesLeaf, DataModelUInt32Leaf &messageRateLeaf,
                       StatsManager &statsManager)
    : bufferPos(0),
      remaining(0),
      carriageReturnFound(false),
      messageHandlers(),
      messagesLeaf(messagesLeaf),
      messageRateLeaf(messageRateLeaf) {
    statsManager.addStatsHolder(*this);
}

void NMEASource::addMessageHandler(NMEAMessageHandler &messageHandler) {
    if (messageHandlers.full()) {
        fatalError("Too many message handles for NMEA source");
    }

    messageHandlers.push_back(&messageHandler);
}


bool NMEASource::scanForCarriageReturn(size_t &carriageReturnPos) {
    size_t scanRemaining;
    size_t scanPos;

    for (scanPos = bufferPos, scanRemaining = remaining;
        scanRemaining;
        scanPos++, scanRemaining--) {
        if (buffer[scanPos] == '\r') {
            carriageReturnPos = scanPos;
            return true;
        }
    }

    return false;
}

bool NMEASource::processBuffer() {
    // Since NMEA 0183 has CR/LF terminated lines, if the last thing we looked at was a carriage
    // return, we should be looking at a line feed.
    if (carriageReturnFound) {
        if (isLineFeed(buffer[bufferPos])) {
            // Skip past the line feed and return saying the line is ready for processing.
            bufferPos++;
            remaining--;
            carriageReturnFound = false;
            return true;
        } else {
            // We had a carriage return without the associated line feed. Toss out any characters
            // that we had accumulated in the line and move on, processing the buffer.
            logger() << logWarnNMEA << "NMEA line with CR, but no LF. Ignoring." << eol;
            inputLine.reset();
            carriageReturnFound = false;
        }
    }

    size_t carriageReturnPos;
    if (scanForCarriageReturn(carriageReturnPos)) {
        // We have a CR, move the characters up to that point into the line.
        inputLine.append(buffer, bufferPos, carriageReturnPos);
        remaining -= carriageReturnPos - bufferPos + 1;
        bufferPos = carriageReturnPos + 1;

        // If we have at least one more character in the buffer make sure it's an LF
        if (remaining) {
            if (isLineFeed(buffer[bufferPos])) {
                // The line was correctly terminated and should be ready to go.
                remaining--;
                bufferPos++;
                return true;
            } else {
                // We had a carriage return without the associated line feed. Toss out any
                // characters that we had accumulated in the line. If we still have characters in
                // the buffer, recursively process those.
                logger() << logWarnNMEA << "NMEA line with CR, but no LF. Ignoring." << eol;
                inputLine.reset();
                remaining--;
                bufferPos++;
                if (remaining) {
                    return processBuffer();
                } else {
                    bufferPos = 0;
                    return false;
                }
            }
        } else {
            // The buffer ended on a carriage return, we'll pick up looking for the line feed after
            // the next read.
            carriageReturnFound = true;
            return false;
        }
    } else {
        // No carriage return so far, stash what we have so far in the line.
        inputLine.append(buffer, bufferPos, bufferPos + remaining);
        bufferPos = 0;
        remaining = 0;
        return false;
    }
}

bool NMEASource::readAvailableInput(int sock) {
    ssize_t count = recv(sock, buffer, maxNMEALineLength, 0);
    if (count == 0) {
        logger() << logWarnNMEA << "NMEA source closed connection." << eol;
        return false;
    } else if (count < 0) {
        logger() << logWarnNMEA << "NMEA source read failed:" << strerror(errno) << "(" << errno
                 << ")" << eol;
        return false;
    } else {
        remaining = count;
        bufferPos = 0;
        return true;
    }
}

void NMEASource::lineCompleted() {
    if (inputLine.isEmpty()) {
        // For now we just ignore empty input lines. Count?
        return;
    }

    if (!inputLine.sanityCheck()) {
        // Errors are logged by the sanity check.
        return;
    }

    NMEAMessage *nmeaMessage = parser.parseLine(inputLine);
    if (nmeaMessage != NULL) {
        nmeaMessage->log();

        for (NMEAMessageHandler *messageHandler : messageHandlers) {
            messageHandler->processMessage(nmeaMessage);
        }

        messagesCounter++;

        // While we're done with the nmeaMessage, we don't do a free here
        // since it was allocated with a static buffer and placement new.
    }
}

void NMEASource::processNMEAStream(int sock) {
    bufferPos = 0;
    remaining = 0;
    carriageReturnFound = false;
    inputLine.reset();

    while (true) {
        if (!readAvailableInput(sock)) {
            return;
        }

        while (remaining) {
            if (processBuffer()) {
                lineCompleted();
                inputLine.reset();
            }
        }
    }
}

void NMEASource::exportStats(uint32_t msElapsed) {
    messagesCounter.update(messagesLeaf, messageRateLeaf, msElapsed);
}
