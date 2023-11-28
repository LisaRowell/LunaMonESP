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

#include "NMEALine.h"
#include "NMEAMessage.h"
#include "NMEATalker.h"
#include "NMEAMsgType.h"
#include "NMEADBKMessage.h"
#include "NMEADBSMessage.h"
#include "NMEADBTMessage.h"
#include "NMEAGGAMessage.h"
#include "NMEAGLLMessage.h"
#include "NMEAGSAMessage.h"
#include "NMEAGSTMessage.h"
#include "NMEAGSVMessage.h"
#include "NMEARMCMessage.h"
#include "NMEATXTMessage.h"
#include "NMEAVDMVDOMessage.h"
#include "NMEAVTGMessage.h"

#include "Logger.h"

#include "etl/string.h"
#include "etl/string_view.h"

NMEAMessage::NMEAMessage(NMEATalker &talker) : talker(talker) {
}

NMEATalker NMEAMessage::source() const {
    return talker;
}

bool NMEAMessage::extractConstantWord(NMEALine &nmeaLine, const char *messageType,
                                      const char *constantWord) {
    etl::string_view word;
    if (!nmeaLine.getWord(word)) {
        logger() << logErrorNMEA << talker << " " << messageType << " message missing "
                 << constantWord << " field" << eol;
        return false;
    }

    if (word != constantWord) {
        logger() << logErrorNMEA << talker << " " << messageType << " message with bad "
                 << constantWord << " field" << eol;
        return false;
    }

    return true;
}

NMEAMessage *parseNMEAMessage(NMEALine &nmeaLine) {
    etl::string_view tagView;
    if (!nmeaLine.getWord(tagView)) {
        logger() << logWarnNMEA << "NMEA message missing tag" << eol;
        return NULL;
    }
    if (tagView.size() != 5) {
        logger() << logWarnNMEA << "Bad NMEA tag '" << tagView << "'" << eol;
        return NULL;
    }

    etl::string<2> talkerCode(tagView.begin(), 2);
    NMEATalker talker(talkerCode);

    etl::string<3> msgTypeStr(tagView.begin() + 2, 3);
    enum NMEAMsgType msgType = parseNMEAMsgType(msgTypeStr);

    switch (msgType) {
        case NMEA_MSG_TYPE_DBK:
            return parseNMEADBKMessage(talker, nmeaLine);

        case NMEA_MSG_TYPE_DBS:
            return parseNMEADBSMessage(talker, nmeaLine);

        case NMEA_MSG_TYPE_DBT:
            return parseNMEADBTMessage(talker, nmeaLine);

        case NMEA_MSG_TYPE_GGA:
            return parseNMEAGGAMessage(talker, nmeaLine);

        case NMEA_MSG_TYPE_GLL:
            return parseNMEAGLLMessage(talker, nmeaLine);

        case NMEA_MSG_TYPE_GSA:
            return parseNMEAGSAMessage(talker, nmeaLine);

        case NMEA_MSG_TYPE_GST:
            return parseNMEAGSTMessage(talker, nmeaLine);

        case NMEA_MSG_TYPE_GSV:
            return parseNMEAGSVMessage(talker, nmeaLine);

        case NMEA_MSG_TYPE_RMC:
            return parseNMEARMCMessage(talker, nmeaLine);

        case NMEA_MSG_TYPE_TXT:
            return parseNMEATXTMessage(talker, nmeaLine);

        case NMEA_MSG_TYPE_VDM:
        case NMEA_MSG_TYPE_VDO:
            return parseNMEAVDMVDOMessage(talker, msgType, nmeaLine);

        case NMEA_MSG_TYPE_VTG:
            return parseNMEAVTGMessage(talker, nmeaLine);

        case NMEA_MSG_TYPE_UNKNOWN:
        default:
            logger() << logWarnNMEA << "Unknown NMEA message type (" << msgTypeStr << ") from "
                     << talker << eol;
            return NULL;
    }
}
