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

#ifndef NMEA_GST_MESSAGE_H
#define NMEA_GST_MESSAGE_H

#include "NMEAMessage.h"
#include "NMEAMsgType.h"
#include "NMEATime.h"
#include "NMEATenthsUInt16.h"

class NMEATalker;
class NMEALineWalker;

class NMEAGSTMessage : public NMEAMessage {
    private:
        NMEATime time;
        NMEATenthsUInt16 standardDeviationOfRangeInputsRMS;
        NMEATenthsUInt16 standardDeviationOfSemiMajorAxis;
        NMEATenthsUInt16 standardDeviationOfSemiMinorAxis;
        NMEATenthsUInt16 orientationOfSemiMajorAxis;
        NMEATenthsUInt16 standardDeviationOfLatitudeError;
        NMEATenthsUInt16 standardDeviationOfLongitudeError;
        NMEATenthsUInt16 standardDeviationOfAltitudeError;

    public:
        NMEAGSTMessage(const NMEATalker &talker);
        bool parse(NMEALineWalker &lineWalker);
        virtual NMEAMsgType::Value type() const override;
        virtual void log() const override;

    friend class GPSBridge;
};

extern NMEAGSTMessage *parseNMEAGSTMessage(const NMEATalker &talker, NMEALineWalker &lineWalker,
                                           uint8_t *nmeaMessageBuffer);

#endif
