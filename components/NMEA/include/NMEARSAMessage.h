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

#ifndef NMEA_RSA_MESSAGE_H
#define NMEA_RSA_MESSAGE_H

#include "NMEAMessage.h"
#include "NMEAMsgType.h"
#include "NMEATenthsInt16.h"
#include "NMEADataValid.h"

class NMEATalker;
class NMEALineWalker;

class NMEARSAMessage : public NMEAMessage {
    private:
        NMEATenthsInt16 starboardRudderSensorAngle;
        NMEADataValid starboardRudderSensorAngleValid;
        NMEATenthsInt16 portRudderSensorAngle;
        NMEADataValid portRudderSensorAngleValid;

    public:
        NMEARSAMessage(const NMEATalker &talker);
        bool parse(NMEALineWalker &lineWalker);
        virtual NMEAMsgType::Value type() const override;
        virtual void log() const override;

    friend class AutoPilotBridge;
};

extern NMEARSAMessage *parseNMEARSAMessage(const NMEATalker &talker, NMEALineWalker &lineWalker,
                                           uint8_t *nmeaMessageBuffer);

#endif // NMEA_RSA_MESSAGE_H
