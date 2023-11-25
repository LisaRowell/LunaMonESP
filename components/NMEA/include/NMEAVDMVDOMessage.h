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

#ifndef NMEA_VDMVDO_MESSAGE_H
#define NMEA_VDMVDO_MESSAGE_H

#include "NMEAMessage.h"
#include "NMEAUInt8.h"
#include "NMEAUInt16.h"
#include "NMEARadioChannelCode.h"
#include "NMEATalker.h"
#include "NMEALine.h"
#include "NMEAMsgType.h"

class NMEAVDMVDOMessage : public NMEAMessage {
    private:
        NMEAMsgType msgType;
        NMEAUInt8 totalFragments;
        NMEAUInt8 fragmentNumber;
        NMEAUInt16 messageId;
        NMEARadioChannelCode radioChannelCode;

        const char *msgTypeName() const;

    public:
        NMEAVDMVDOMessage(NMEATalker &talker, NMEAMsgType &msgType);
        bool parse(NMEALine &nmeaLine);
        virtual enum NMEAMsgType type() const override;
        virtual void log() const override;
        bool isOwnShip() const;

    friend class NMEADataModelBridge;
};

extern NMEAVDMVDOMessage *parseNMEAVDMVDOMessage(NMEATalker &talker, NMEAMsgType &msgType,
                                                 NMEALine &nmeaLine);

#endif
