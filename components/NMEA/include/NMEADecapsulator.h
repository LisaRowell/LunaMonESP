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

#ifndef NMEA_DECAPSULATOR_H
#define NMEA_DECAPSULATOR_H

#include "NMEAMsgType.h"
#include "NMEATalker.h"

#include "etl/array.h"
#include "etl/bit_stream.h"
#include "etl/string_view.h"

#include <stdint.h>
#include <stddef.h>

class NMEATalker;

constexpr size_t maxNMEAEncapsulatedMessageSize = 256;

class NMEADecapsulator {
    private:
        bool decapsulationInProgress;
        NMEATalker messageTalker;
        enum NMEAMsgType messageMsgType;
        uint8_t messageFragmentCount;
        uint8_t lastFragmentIndex;
        uint32_t messageID;
        etl::array<uint8_t, maxNMEAEncapsulatedMessageSize> messageDataArray;
        etl::bit_stream_writer messageBitStream;

        bool fragmentIsNext(NMEATalker &talker, enum NMEAMsgType msgType, uint8_t fragmentCount,
                            uint8_t fragmentIndex, uint32_t messageIdOrZero);
        bool addFragmentPayload(etl::string_view &payloadView, uint8_t fillBits);
        uint8_t payloadCharValue(char payloadChar);

    public:
        NMEADecapsulator();
        void addFragment(NMEATalker &talker, enum NMEAMsgType msgType, uint8_t fragmentCount,
                         uint8_t fragmentIndex, uint32_t messageIdOrZero,
                         etl::string_view &payloadView, uint8_t fillBits);
        bool isComplete() const;
        const etl::array<uint8_t, maxNMEAEncapsulatedMessageSize> &messageData() const;
        size_t messageBitLength() const;
        size_t messageByteLength() const;
        void reset();
};

#endif // NMEA_DECAPSULATOR_H
