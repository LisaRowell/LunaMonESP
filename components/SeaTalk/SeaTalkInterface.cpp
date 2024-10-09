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

#include "SeaTalkInterface.h"
#include "SeaTalkWriteTester.h"
#include "Interface.h"

#include "DataModelNode.h"
#include "DataModelUInt32Leaf.h"
#include "StatsManager.h"
#include "StatCounter.h"

#include "Logger.h"
#include "Error.h"

SeaTalkInterface::SeaTalkInterface(Interface &interface, InstrumentData &instrumentData,
                                   StatsManager &statsManager)
    : interface(interface),
      parser(interface.interfaceNode(), instrumentData, statsManager),
      writeTester(nullptr),
      inputDatagramCounter(),
      mergedDatagrams(0),
      outputDatagramCounter(),
      outputErrors(0),
      seaTalkNode("seaTalk", &interface.interfaceNode()),
      inputNode("input", &seaTalkNode),
      inputDatagramsLeaf("datagrams", &inputNode),
      inputDatagramsRateLeaf("datagramRate", &inputNode),
      mergedDatagramsLeaf("mergedDatagrams", &inputNode),
      outputNode("output", &seaTalkNode),
      outputDatagramsLeaf("datagrams", &outputNode),
      outputDatagramsRateLeaf("datagramRate", &outputNode),
      outputErrorsLeaf("errors", &outputNode) {
    statsManager.addStatsHolder(*this);

    if (CONFIG_LUNAMON_SEA_TALK_WRITE_TEST_ENABLED) {
        writeTester = new SeaTalkWriteTester(*this);
        if (writeTester == nullptr) {
            logger() << logErrorSeaTalk << "Failed to allocate SeaTalk write tester" << eol;
            errorExit();
        }
    }
}

void SeaTalkInterface::start() {
    if (writeTester) {
        writeTester->start();
    }
}

void SeaTalkInterface::processBuffer(uint16_t *buffer, size_t length) {
    for (int pos = 0; pos < length; pos++) {
        uint16_t nextChar = buffer[pos];
        if (nextChar & 0x100) {
            if (!inputLine.isEmpty()) {
                mergedDatagrams++;
                // It might be better to make this a debug message if it occures a lot.
                logger() << logWarnSeaTalk
                         << "Merged SeaTalk datagram...ignoring truncated datagram: " << inputLine
                         << eol;
                inputLine.clear();
            }
            inputLine.append((uint8_t)(nextChar & 0x0ff));
        } else {
            inputLine.append((uint8_t)nextChar);
            if (inputLine.isComplete()) {
                logger() << logDebugSeaTalk << "Received datagram from SeaTalk interface "
                         << interface.name() << ": " << inputLine << eol;
                parser.parseLine(inputLine);
                inputDatagramCounter++;
                inputLine.clear();
            }
        }
    }
}

// Not called on the interface task!
void SeaTalkInterface::sendCommand(const SeaTalkLine &seaTalkLine) {
    taskLogger() << logDebugSeaTalk << "Sending SeaTalk command: " << seaTalkLine << eol;

    // On the wire, a SeaTalk command is encoded into 9-bit characters with the msb used to indicate
    // the first character in the line (the command byte).
    uint16_t buffer[SeaTalkLine::maxLineLength];
    size_t length = seaTalkLine.buildNineBitString(buffer);

    if (interface.sendBytes(buffer, length) != length) {
        taskLogger() << logWarnSeaTalk << "Failed to send command to SeaTalk interface "
                     << interface.name() << eol;
        outputErrors++;
    }
}

// For STALK interfaces this is called by STALKInterface::exportStats()
void SeaTalkInterface::exportStats(uint32_t msElapsed) {
    inputDatagramCounter.update(inputDatagramsLeaf, inputDatagramsRateLeaf, msElapsed);
    mergedDatagramsLeaf = mergedDatagrams;

    outputDatagramCounter.update(outputDatagramsLeaf, outputDatagramsRateLeaf, msElapsed);
    outputErrorsLeaf = outputErrors;
}
