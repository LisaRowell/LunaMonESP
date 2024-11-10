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
#include "NMEALineHandler.h"
#include "NMEALineWalker.h"
#include "NMEATalker.h"
#include "NMEAMsgType.h"

#include "DataModelNode.h"
#include "DataModelStringLeaf.h"
#include "DataModelUInt32Leaf.h"

#include "StatsManager.h"
#include "CharacterTools.h"
#include "Logger.h"
#include "Error.h"

#include "etl/vector.h"
#include "etl/set.h"
#include "etl/string.h"
#include "etl/string_view.h"

#include <stddef.h>
#include <string.h>

NMEALineSource::NMEALineSource(DataModelNode &interfaceNode, const char *filteredTalkersList,
                               StatsManager &statsManager)
    : carriageReturnFound(false),
      messagesCounter(),
      talkerFilteredMessages(0),
      badTagMessages(0),
      nmeaNode("nmea", &interfaceNode),
      _nmeaInputNode("input", &nmeaNode),
      messagesLeaf("messages", &_nmeaInputNode),
      messageRateLeaf("messageRate", &_nmeaInputNode),
      talkersLeaf("talkers", &_nmeaInputNode, talkersBuffer),
      talkerFilteredMessagesLeaf("talkerFilteredMsgs", &_nmeaInputNode),
      badTagsMessagesLeaf("badTagMsgs", &_nmeaInputNode) {
    statsManager.addStatsHolder(*this);
    talkersLeaf = "";

    buildFilteredTalkersSet(filteredTalkersList);
}

void NMEALineSource::buildFilteredTalkersSet(const char *filteredTalkersList) {
    etl::string_view filteredTalkersListView(filteredTalkersList);

    while (!filteredTalkersListView.empty()) {
        etl::string_view talkerStrView;

        size_t commaPos = filteredTalkersListView.find(',');
        if (commaPos == filteredTalkersListView.npos) {
            talkerStrView.assign(filteredTalkersListView.begin(), filteredTalkersListView.end());
            filteredTalkersListView.remove_prefix(filteredTalkersListView.size());
        } else {
            talkerStrView.assign(filteredTalkersListView.begin(),
                                 filteredTalkersListView.begin() + commaPos);
            filteredTalkersListView.remove_prefix(commaPos + 1);
        }

        if (talkerStrView.length() != 2) {
            logger() << logWarnNMEALine << "Ignoring bad NMEA talker code '" << talkerStrView
                     << "' in NMEA 0183 talker filter list: " << filteredTalkersList << eol;
        } else {
            NMEATalker talker(talkerStrView);

            if (filteredTalkers.contains(talker)) {
                logger() << logWarnNMEALine << "Duplicate NMEA talker '" << talkerStrView
                        << "' in NMEA 0183 talker filter list: " << filteredTalkersList << eol;
            } else {
                filteredTalkers.insert(talker);
            }
        }
    }
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

    messagesCounter++;

    NMEATalker talker;
    NMEAMsgType msgType;
    if (parseTag(talker, msgType)) {
        if (!talkers.contains(talker)) {
            newTalkerSeen(talker);
        }

        if (filteredTalkers.contains(talker)) {
            messageFilteredByTalker(talker);
        } else {
            handleLine(talker, msgType);
        }
    }
}

bool NMEALineSource::parseTag(NMEATalker &talker, NMEAMsgType &msgType) {
    NMEALineWalker walker(inputLine, true);

    etl::string_view tagView;
    if (!walker.getWord(tagView)) {
        logger() << logWarnNMEALine << "NMEA message missing tag" << eol;
        badTagMessages++;
        return false;
    }

    // Deal with the proprietary messages which have a four character tag instead of the usual five
    if (tagView.size() == 4 && tagView[0] == 'P') {
        talker = NMEATalker("PD");
        msgType = NMEAMsgType(NMEAMsgType::PROPRIETARY);
        return true;
    }

    if (tagView.size() != 5) {
        logger() << logWarnNMEALine << "Bad NMEA tag '" << tagView << "'" << eol;
        badTagMessages++;
        return false;
    }

    etl::string<2> talkerCode(tagView.begin(), 2);
    talker = NMEATalker(talkerCode);

    etl::string<3> msgTypeStr(tagView.begin() + 2, 3);
    msgType = NMEAMsgType(msgTypeStr);
    if (msgType == NMEAMsgType::UNKNOWN) {
        logger() << logWarnNMEALine << "NMEA message with unknown type (" << msgTypeStr << ")"
                 << " from " << talker << ". Ignored." << eol;
        return false;
    }

    return true;
}

void NMEALineSource::newTalkerSeen(const NMEATalker &talker) {
    if (!talkers.full()) {
        logger() << logNotifyNMEA << "New NMEA talker '" << talker << "'" << eol;

        talkers.insert(talker);

        etl::string<4> newText;
        if (!talkersLeaf.isEmptyStr()) {
            newText = ",";
        }
        newText.append(talker.name());
        talkersLeaf.append(newText);
    } else {
        logger() << logDebugNMEA << "Maximum NMEA talkers exceeded. Talker '" << talker
                 << "' not reported." << eol;
    }
}

void NMEALineSource::messageFilteredByTalker(const NMEATalker &talker) {
    talkerFilteredMessages++;

    logger() << logDebugNMEA << "Filtering NMEA msg from talker '" << talker << "': " << inputLine
             << eol;
}

void NMEALineSource::handleLine(const NMEATalker &talker, const NMEAMsgType &msgType) {
    logger() << logDebugNMEALine << "Handling NMEA formatted line: " << inputLine << eol;
    for (NMEALineHandler *lineHandler : lineHandlers) {
        lineHandler->handleLine(inputLine, talker, msgType);
    }
}

void NMEALineSource::addLineHandler(NMEALineHandler &lineHandler) {
    if (lineHandlers.full()) {
        logger() << logErrorNMEALine << "Attempt to add more than " << MAX_LINE_HANDLERS
                 << " line handlers to a NMEALineSource" << eol;
        errorExit();
    }

    lineHandlers.push_back(&lineHandler);
}

DataModelNode &NMEALineSource::nmeaInputNode() {
    return _nmeaInputNode;
}

void NMEALineSource::exportStats(uint32_t msElapsed) {
    messagesCounter.update(messagesLeaf, messageRateLeaf, msElapsed);
    talkerFilteredMessagesLeaf = talkerFilteredMessages;
    badTagsMessagesLeaf = badTagMessages;
}
