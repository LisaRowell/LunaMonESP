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

#include "NMEAMessage.h"

#include "NMEALineWalker.h"
#include "NMEATalker.h"
#include "NMEAMsgType.h"

#include "Logger.h"

#include "etl/string_view.h"

NMEAMessage::NMEAMessage(const NMEATalker &talker) : talker(talker) {
}

const NMEATalker &NMEAMessage::source() const {
    return talker;
}

const char *NMEAMessage::typeName() const {
    NMEAMsgType msgType(type());
    return msgType.name();
}

bool NMEAMessage::extractConstantWord(NMEALineWalker &lineWalker, const char *messageType,
                                      const char *constantWord, bool optional) {
    etl::string_view word;
    if (!lineWalker.getWord(word)) {
        logger() << logErrorNMEA << talker << " " << messageType << " message missing "
                 << constantWord << " field" << eol;
        return false;
    }

    if ((word != constantWord) && !(word.length() == 0 && optional)) {
        logger() << logErrorNMEA << talker << " " << messageType << " message with bad "
                 << constantWord << " field" << eol;
        return false;
    }

    return true;
}
