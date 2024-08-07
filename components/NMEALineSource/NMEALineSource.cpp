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

#include "NMEALineSource.h"
#include "NMEALine.h"

#include "CharacterTools.h"
#include "Logger.h"
#include "Error.h"

#include <stddef.h>
#include <string.h>

NMEALineSource::NMEALineSource()
    : carriageReturnFound(false) {
}

void NMEALineSource::sourceReset() {
    carriageReturnFound = false;
    inputLine.reset();
}

bool NMEALineSource::scanForCarriageReturn(size_t &carriageReturnPos, const char *buffer,
                                           size_t &bufferPos, size_t &remaining) {
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

bool NMEALineSource::processBufferToEndOfLine(const char *buffer, size_t &bufferPos,
                                              size_t &remaining) {
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
            logger() << logWarnNMEALine << "NMEA line with CR, but no LF. Ignoring." << eol;
            inputLine.reset();
            carriageReturnFound = false;
        }
    }

    size_t carriageReturnPos;
    if (scanForCarriageReturn(carriageReturnPos, buffer, bufferPos, remaining)) {
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
                logger() << logWarnNMEALine << "NMEA line with CR, but no LF. Ignoring." << eol;
                inputLine.reset();
                remaining--;
                bufferPos++;
                if (remaining) {
                    return processBufferToEndOfLine(buffer, bufferPos, remaining);
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

void NMEALineSource::processBuffer(const char *buffer, size_t length) {
    size_t bufferPos = 0;
    size_t remaining = length;

    while (remaining) {
        if (processBufferToEndOfLine(buffer, bufferPos, remaining)) {
            lineCompleted();
            inputLine.reset();
        }
    }
}

void NMEALineSource::lineCompleted() {
    if (inputLine.isEmpty()) {
        // For now we just ignore empty input lines. Count?
        return;
    }

    if (!inputLine.sanityCheck()) {
        // Errors are logged by the sanity check.
        return;
    }

    handleLine(inputLine);
}
