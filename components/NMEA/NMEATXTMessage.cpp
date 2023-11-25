/*
 * This file is part of LunaMon (https://github.com/LisaRowell/LunaMon)
 * Copyright (C) 2021-2023 Lisa Rowell
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

#include "NMEATXTMessage.h"
#include "NMEAMsgType.h"
#include "NMEAMessage.h"
#include "NMEALine.h"
#include "NMEATalker.h"
#include "NMEAMessageBuffer.h"

#include "Logger.h"

#include <etl/string_view.h>
#include <etl/to_arithmetic.h>

NMEATXTMessage::NMEATXTMessage(NMEATalker &talker) : NMEAMessage(talker) {
}

bool NMEATXTMessage::parse(NMEALine &nmeaLine) {
    if (nmeaLine.isEncapsulatedData()) {
        logger() << logWarnNMEA << talker << " TXT message in unsupported encapsulated format"
                 << eol;
        return false;
    }

    if (!getTwoDigitField(nmeaLine, totalSentences, "Total Sentences")) {
        return false;
    }

    if (!getTwoDigitField(nmeaLine, sentenceNumber, "Sentence Number")) {
        return false;
    }

    if (!getTwoDigitField(nmeaLine, textIdentifier, "Text Identifier")) {
        return false;
    }

    etl::string_view textView;
    if (!nmeaLine.getWord(textView)) {
        logger() << logWarnNMEA << talker << " TXT message missing Text field" << eol;
        return false;
    }
    text.assign(textView.begin(), textView.end());

    return true;
}

bool NMEATXTMessage::getTwoDigitField(NMEALine &nmeaLine, uint8_t &value, const char *fieldName) {
    etl::string_view valueView;
    if (!nmeaLine.getWord(valueView)) {
        logger() << logWarnNMEA << talker << " TXT message missing " << fieldName << " field"
                 << eol;
        return false;
    }

    etl::to_arithmetic_result<uint8_t> conversionResult;
    conversionResult = etl::to_arithmetic<uint8_t>(valueView);
    if (!conversionResult.has_value()) {
        logger() << logWarnNMEA << talker << " TXT message with bad " << fieldName << " field '"
                 << valueView << "'" << eol;
        return false;
    }
    value = conversionResult.value();
    return true;
}

enum NMEAMsgType NMEATXTMessage::type() const {
    return NMEA_MSG_TYPE_TXT;
}

void NMEATXTMessage::log() const {
    logger() << logDebugNMEA << talker << " TXT: TotalSentences " << totalSentences << " Sentence "
             << sentenceNumber << " TextId " << textIdentifier << " " << text << eol;
}

NMEATXTMessage *parseNMEATXTMessage(NMEATalker &talker, NMEALine &nmeaLine) {
    NMEATXTMessage *message = new (nmeaMessageBuffer)NMEATXTMessage(talker);
    if (!message) {
        return NULL;
    }

    if (!message->parse(nmeaLine)) {
        // Since we use a static buffer and placement new for messages, we don't do a free here.
        return NULL;
    }

    return message;
}
