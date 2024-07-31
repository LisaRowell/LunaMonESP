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

#include "NMEAVDMMessage.h"
#include "NMEAVDMVDOMessage.h"
#include "NMEATalker.h"
#include "NMEAMsgType.h"
#include "NMEAMessageBuffer.h"

#include "Logger.h"

#include "etl/bit_stream.h"

#include "stddef.h"

NMEAVDMMessage::NMEAVDMMessage(const NMEATalker &talker) : NMEAVDMVDOMessage(talker) {
}

NMEAMsgType::Value NMEAVDMMessage::type() const {
    return NMEAMsgType::VDM;
}

void NMEAVDMMessage::log() const {
    logAIS("VDM");
}

NMEAVDMMessage *parseVDMMessage(const NMEATalker &talker, etl::bit_stream_reader &streamReader,
                                size_t messageSizeInBits, AISContacts &aisContacts) {
    NMEAVDMMessage *message = new (nmeaMessageBuffer)NMEAVDMMessage(talker);
    if (!message) {
        return nullptr;
    }

    if (!message->parse(streamReader, messageSizeInBits, false, aisContacts)) {
        // Since we use a static buffer and placement new for messages, we don't do a free here.
        return nullptr;
    }

    return message;
}
