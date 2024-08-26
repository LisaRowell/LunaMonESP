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

#include "NMEAGSTMessage.h"
#include "NMEAMsgType.h"
#include "NMEATalker.h"
#include "NMEAMsgType.h"
#include "NMEALineWalker.h"
#include "NMEATime.h"
#include "NMEATenthsUInt16.h"

#include "StringTools.h"
#include "Logger.h"

NMEAGSTMessage::NMEAGSTMessage(const NMEATalker &talker) : NMEAMessage(talker) {
}

bool NMEAGSTMessage::parse(NMEALineWalker &lineWalker) {
    if (!time.extract(lineWalker, talker, "GST")) {
        return false;
    }

    if (!standardDeviationOfRangeInputsRMS.extract(lineWalker, talker, "GST",
                                                   "RMS Value of Standard Deviation of Range "
                                                   "Inputs")) {
        return false;
    }

    if (!standardDeviationOfSemiMajorAxis.extract(lineWalker, talker, "GST",
                                                  "Standard Deviation of Semi-Major Axis of Error "
                                                  "Ellipse", true)) {
        return false;
    }

    if (!standardDeviationOfSemiMinorAxis.extract(lineWalker, talker, "GST",
                                                  "Standard Deviation of Semi-Minor Axis of Error "
                                                  "Ellipse", true)) {
        return false;
    }

    if (!orientationOfSemiMajorAxis.extract(lineWalker, talker, "GST",
                                            "Orientation of Semi-Major Axis of Error Ellipse",
                                            true)) {
        return false;
    }

    if (!standardDeviationOfLatitudeError.extract(lineWalker, talker, "GST",
                                                  "Standard Deviation of Latitude Error")) {
        return false;
    }

    if (!standardDeviationOfLongitudeError.extract(lineWalker, talker, "GST",
                                                   "Standard Deviation of Longitude Error")) {
        return false;
    }

    if (!standardDeviationOfAltitudeError.extract(lineWalker, talker, "GST",
                                                  "Standard Deviation of Altitude Error")) {
        return false;
    }

    return true;
}

NMEAMsgType::Value NMEAGSTMessage::type() const {
    return NMEAMsgType::GST;
}

void NMEAGSTMessage::log() const {
    logger() << logDebugNMEA << talker << " GST: " << time << " SD of Range of Inputs RMS "
             << standardDeviationOfRangeInputsRMS << " SD of Semi-Major Axis "
             << standardDeviationOfSemiMajorAxis << "m SD of Semi-Minor Axis "
             << standardDeviationOfSemiMinorAxis << "m Orientation of Semi Major Axis "
             << orientationOfSemiMajorAxis << " SD of Latitude Error "
             << standardDeviationOfLatitudeError << "m SD of Longitude Error "
             << standardDeviationOfLongitudeError << "m SD of Atitude Error "
             << standardDeviationOfAltitudeError << "m" << eol;
}

NMEAGSTMessage *parseNMEAGSTMessage(const NMEATalker &talker, NMEALineWalker &lineWalker,
                                    uint8_t *nmeaMessageBuffer) {
    NMEAGSTMessage *message = new (nmeaMessageBuffer)NMEAGSTMessage(talker);
    if (!message) {
        return nullptr;
    }

    if (!message->parse(lineWalker)) {
        // Since we use a static buffer and placement new for messages, we don't do a free here.
        return nullptr;
    }

    return message;
}
