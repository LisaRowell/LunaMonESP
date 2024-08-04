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

#include "NMEAVDOMessage.h"
#include "NMEAVDMVDOMessage.h"
#include "NMEATalker.h"
#include "NMEAMsgType.h"

#include "Logger.h"

#include "etl/bit_stream.h"

#include "stddef.h"

NMEAVDOMessage::NMEAVDOMessage(const NMEATalker &talker) : NMEAVDMVDOMessage(talker) {
}

NMEAMsgType::Value NMEAVDOMessage::type() const {
    return NMEAMsgType::VDO;
}

void NMEAVDOMessage::log() const {
    logAIS("VDO");
}

NMEAVDOMessage *parseVDOMessage(const NMEATalker &talker, etl::bit_stream_reader &streamReader,
                                size_t messageSizeInBits, AISContacts &aisContacts,
                                uint8_t *nmeaMessageBuffer) {
    NMEAVDOMessage *message = new (nmeaMessageBuffer)NMEAVDOMessage(talker);
    if (!message) {
        return nullptr;
    }

    if (!message->parse(streamReader, messageSizeInBits, true, aisContacts)) {
        // Since we use a static buffer and placement new for messages, we don't do a free here.
        return nullptr;
    }

    return message;
}
