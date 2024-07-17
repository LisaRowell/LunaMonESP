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

#include "NMEADecapsulator.h"
#include "NMEATalker.h"
#include "NMEAMsgType.h"

#include "Logger.h"

#include "etl/string_view.h"
#include "etl/array.h"
#include "etl/bit_stream.h"
#include "etl/endianness.h"

#include <stdint.h>

NMEADecapsulator::NMEADecapsulator() : messageBitStream(messageDataArray.data(),
                                                        messageDataArray.size(),
                                                        etl::endian::big) {
    reset();
}

void NMEADecapsulator::addFragment(NMEATalker &talker, enum NMEAMsgType msgType,
                                   uint8_t fragmentCount, uint8_t fragmentIndex,
                                   uint32_t messageIdOrZero, etl::string_view &payloadView,
                                   uint8_t fillBits) {
    if (decapsulationInProgress &&
        !fragmentIsNext(talker, msgType, fragmentCount, fragmentIndex, messageIdOrZero)) {
        // We're working with a fragment that does not match an decapsulation that's in progress.
        // Ditch the old message, but fall through and work on the fragment we've been given as it
        // might be the start of a different message.
        logger() << logWarnNMEA << "Incomplete encapsulated NMEA "<< messageMsgType
                 << " message from " << messageTalker << eol;
        decapsulationInProgress = false;
    }

    if (!decapsulationInProgress) {
        if (fragmentIndex != 1) {
            // We came into the middle of a message's fragment stream so we should discard this
            // fragment as it's useless without the preceeding ones.
            logger() << logWarnNMEA << "Discarding encapsulated NMEA " << nmeaMsgTypeName(msgType)
                     << " message fragment from " << talker
                     << " that is missing preceeding fragments" << eol;
            return;
        }

        messageTalker = talker;
        messageMsgType = msgType;
        messageFragmentCount = fragmentCount;
        messageID = messageIdOrZero;
        decapsulationInProgress = true;
    }

    lastFragmentIndex = fragmentIndex;

    if (!addFragmentPayload(payloadView, fillBits)) {
        reset();
    }
}

bool NMEADecapsulator::fragmentIsNext(NMEATalker &talker, enum NMEAMsgType msgType,
                                      uint8_t fragmentCount, uint8_t fragmentIndex,
                                      uint32_t messageIdOrZero) {
    return messageTalker == talker &&
           messageMsgType == msgType &&
           messageFragmentCount == fragmentCount &&
           lastFragmentIndex + 1 == fragmentIndex &&
           messageID == messageIdOrZero;
}

bool NMEADecapsulator::isComplete() const {
    return lastFragmentIndex == messageFragmentCount;
}

const etl::array<uint8_t, maxNMEAEncapsulatedMessageSize> &NMEADecapsulator::messageData() const {
    return messageDataArray;
}

size_t NMEADecapsulator::messageBitLength() const {
    return messageBitStream.size_bits();
}

size_t NMEADecapsulator::messageByteLength() const {
    return messageBitStream.size_bytes();
}

void NMEADecapsulator::reset() {
    decapsulationInProgress = false;
    messageBitStream.restart();
}

bool NMEADecapsulator::addFragmentPayload(etl::string_view &payloadView, uint8_t fillBits) {
    for (auto payloadItr = payloadView.begin(); payloadItr < payloadView.end(); payloadItr++) {
        uint8_t payloadValue = payloadCharValue(*payloadItr);

        // The last byte of payload can have lower order bits that are padding and shouldn't be
        // added to the stream.
        uint8_t validBits; 
        if (payloadItr + 1 == payloadView.end()) {
            validBits = 6 - fillBits;
            payloadValue = payloadValue >> fillBits;
        } else {
            validBits = 6;
        }

        if (!messageBitStream.write(payloadValue, validBits)) {
            logger() << logWarnNMEA << "Failed to add encapsulated fragment payload to message"
                     << eol;
            return false;
        }
    }

    return true;
}

uint8_t NMEADecapsulator::payloadCharValue(char payloadChar) {
    // From https://gpsd.gitlab.io/gpsd/AIVDM.html "AIVDM/AIVDO Payload Armoring"
    uint8_t value = payloadChar - 48;
    if (value > 40) {
        value -= 8;
    }
    return value;
}
