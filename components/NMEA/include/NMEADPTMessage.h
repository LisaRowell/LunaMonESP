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

#ifndef NMEA_DPT_MESSAGE_H
#define NMEA_DPT_MESSAGE_H

#include "NMEAMessage.h"
#include "NMEAMsgType.h"

#include "NMEATenthsInt16.h"
#include "NMEATenthsUInt16.h"

class NMEATalker;
class NMEALine;

class NMEADPTMessage : public NMEAMessage {
    private:
        NMEATenthsUInt16 depthBelowTransducerMeters;
        NMEATenthsInt16 transducerOffsetMeters;
        NMEATenthsUInt16 maxRangeScaleMeters;

    public:
        NMEADPTMessage(const NMEATalker &talker);
        bool parse(NMEALine &nmeaLine);
        virtual NMEAMsgType::Value type() const override;
        virtual void log() const override;

    friend class DepthBridge;
};

extern NMEADPTMessage *parseNMEADPTMessage(const NMEATalker &talker, NMEALine &nmeaLine,
                                           uint8_t *nmeaMessageBuffer);

#endif // NMEA_DPT_MESSAGE_H
