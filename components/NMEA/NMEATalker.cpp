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

#include "NMEATalker.h"

#include "Logger.h"
#include "Error.h"

#include "etl/string.h"

typedef struct {
    const char code[3];
    const char *description;
} TalkerTableEntry;

// This table is reasonably complete with the goal of making debugging what's on older boats easier.
// While the table is only looked at while generating debug messages, entries that are common
// should be moved to the top. Alternatively they could be put back into alphabetical order and a
// binary search used.
static const TalkerTableEntry talkerTable[] = {
    { "GP", "GPS" },
    { "AI", "AIS" },
    { "AB", "Independent AIS Base Station" },
    { "AD", "Dependent AIS Base Station" },
    { "AG", "Autopilot - General" },
    { "AI", "Mobile AIS Station" },
    { "AN", "AIS Aid to Navigation" },
    { "AP", "Autopilot - Magnetic" },
    { "AR", "AIS Receiving Station" },
    { "AT", "AIS Transmitting Station" },
    { "AX", "AIS Simplex Repeater" },
    { "BD", "BeiDou" },
    { "BI", "Bilge System" },
    { "BN", "Bridge navigational watch alarm system" },
    { "CA", "Central Alarm" },
    { "CC", "Computer - Programmed Calculator" },
    { "CD", "DSC" },
    { "CM", "Computer - Memory Data" },
    { "CR", "Data Receiver" },
    { "CS", "Communications - Satellite" },
    { "CT", "Communications - Radio-Telephone (MF/HF)" },
    { "CV", "Communications - Radio-Telephone (VHF)" },
    { "CX", "Communications - Scanning Receiver" },
    { "DE", "DECCA Navigation" },
    { "DF", "Direction Finder" },
    { "DM", "Velocity Sensor, Speed Log, Water, Magnetic" },
    { "DP", "Dynamiv Position" },
    { "DU", "Duplex repeater station" },
    { "EC", "ECDIS" },
    { "EP", "EPIRB" },
    { "ER", "Engine Room Monitoring Systems" },
    { "FD", "Fire Door" },
    { "FS", "Fire Sprinkler" },
    { "GA", "Galileo Positioning System" },
    { "GB", "BeiDou" },
    { "GI", "NavIC, IRNSS" },
    { "GL", "GLONASS" },
    { "GN", "Combination of multiple satellite systems" },
    { "GQ", "QZSS regional GPS augmentation system" },
    { "HC", "Heading - Magnetic Compass" },
    { "HD", "Hull Door" },
    { "HE", "Heading - North Seeking Gyro" },
    { "HF", "Heading - Fluxgate" },
    { "HN", "Heading - Non North Seeking Gyro" },
    { "HS", "Hull Stress" },
    { "II", "Integrated Instrumentation" },
    { "IN", "Integrated Navigation" },
    { "JA", "Alarm and Monitoring" },
    { "JB", "Water Monitoring" },
    { "JC", "Power Management" },
    { "JD", "Propulsion Control" },
    { "JE", "Engine Control" },
    { "JF", "Propulsion Boiler" },
    { "JG", "Aux Boiler" },
    { "JH", "Engine Governor" },
    { "LA", "Loran A" },
    { "LC", "Loran C" },
    { "MP", "Microwave Positioning System" },
    { "MX", "Multiplexer" },
    { "NL", "Navigation light controller" },
    { "OM", "OMEGA Navigation System" },
    { "OS", "Distress Alarm System" },
    { "QZ", "QZSS regional GPS augmentation system" },
    { "RA", "RADAR and/or ARPA" },
    { "RB", "Record Book" },
    { "RC", "Propulsion Machinery" },
    { "RI", "Rudder Angle Indicator" },
    { "SA", "Physical Shore AIS Station" },
    { "SD", "Depth Sounder" },
    { "SG", "Steering Gear" },
    { "SN", "Electronic Positioning System, other/general" },
    { "SS", "Scanning Sounder" },
    { "ST", "Skytraq debug output" },
    { "TC", "Track Control" },
    { "TI", "Turn Rate Indicator" },
    { "TR", "TRANSIT Navigation System" },
    { "U0", "User 0" },
    { "U1", "User 1" },
    { "U2", "User 2" },
    { "U3", "User 3" },
    { "U4", "User 4" },
    { "U5", "User 5" },
    { "U6", "User 6" },
    { "U7", "User 7" },
    { "U8", "User 8" },
    { "U9", "User 9" },
    { "UP", "Microprocessor controller" },
    { "VA", "VHF Data Exchange System (VDES), ASM" },
    { "VD", "Velocity Sensor, Doppler, other/general" },
    { "VM", "Velocity Sensor, Speed Log, Water, Magnetic" },
    { "VR", "Voyage Data recorder" },
    { "VS", "VHF Data Exchange System (VDES), Satellite" },
    { "VT", "VHF Data Exchange System (VDES), Terrestrial" },
    { "VW", "Velocity Sensor, Speed Log, Water, Mechanical" },
    { "WD", "Watertight Door" },
    { "WI", "Weather Instruments" },
    { "WL", "Water Level" },
    { "YC", "Transducer - Temperature" },
    { "YD", "Transducer - Displacement, Angular or Linear" },
    { "YF", "Transducer - Frequency" },
    { "YL", "Transducer - Level" },
    { "YP", "Transducer - Pressure" },
    { "YR", "Transducer - Flow Rate" },
    { "YT", "Transducer - Tachometer" },
    { "YV", "Transducer - Volume" },
    { "YX", "Transducer" },
    { "ZA", "Timekeeper - Atomic Clock" },
    { "ZC", "Timekeeper - Chronometer" },
    { "ZQ", "Timekeeper - Quartz" },
    { "ZV", "Timekeeper - Radio Update, WWV or WWVH" },
    { "", NULL }
};

NMEATalker::NMEATalker(const etl::istring &talkerCode) {

    if (talkerCode.is_truncated() || talkerCode.size() != 2) {
        fatalError("Bad parsing of the NMEA Talker Code");
    }

    this->talkerCode = talkerCode;
}

const char *NMEATalker::name() const {
    unsigned tableEntryIndex;
    for (tableEntryIndex = 0; talkerTable[tableEntryIndex].code[0] != 0; tableEntryIndex++) {
        const TalkerTableEntry &tableEntry = talkerTable[tableEntryIndex];
        if (tableEntry.code[0] == talkerCode[0] && tableEntry.code[1] == talkerCode[1]) {
            return tableEntry.description;
        }
    }

    if (talkerCode[0] == 'P') {
        return "Proprietary";
    }

    return "Unknown";
}

void NMEATalker::log(Logger &logger) const {
    logger << name();
}
