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

#include "NMEAVDMAidToNavigationMsg.h"
#include "NMEAMsgType.h"

#include "etl/bit_stream.h"

#include <stddef.h>

NMEAVDMAidToNavigationMsg::NMEAVDMAidToNavigationMsg(NMEATalker &talker) : NMEAMessage(talker) {
}

bool NMEAVDMAidToNavigationMsg::parse(etl::bit_stream_reader &streamReader,
                                      size_t messageSizeInBits) {
    if (messageSizeInBits < 272 || messageSizeInBits > 360) {
        logger() << logWarnNMEA << talker << " VDM Aid-To-Navigation Report Msg with bad length ("
                 << messageSizeInBits << ")" << eol;
        return false;
    }

    repeatIndicator = etl::read_unchecked<uint8_t>(streamReader, 2);
    mmsi = etl::read_unchecked<uint32_t>(streamReader, 30);
    navAidType.parse(streamReader);
    name.parse(streamReader);
    positionAccuracy = etl::read_unchecked<bool>(streamReader);
    longitudeTenThousandsNM = etl::read_unchecked<int32_t>(streamReader, 28);
    latitudeTenThousandsNM = etl::read_unchecked<int32_t>(streamReader, 27);
    dimensionToBowM = etl::read_unchecked<uint16_t>(streamReader, 9);
    dimensionToSternM = etl::read_unchecked<uint16_t>(streamReader, 9);
    dimensionToPortM = etl::read_unchecked<uint8_t>(streamReader, 6);
    dimensionToStarboardM = etl::read_unchecked<uint8_t>(streamReader, 6);
    epfdFixType.parse(streamReader);
    utcSecond = etl::read_unchecked<uint8_t>(streamReader, 6);
    offPosition = etl::read_unchecked<bool>(streamReader);
    regionalReserved = etl::read_unchecked<uint8_t>(streamReader);
    raim = etl::read_unchecked<bool>(streamReader);
    virtualAid = etl::read_unchecked<bool>(streamReader);
    assignedMode = etl::read_unchecked<bool>(streamReader);
    spare = etl::read_unchecked<uint8_t>(streamReader, 1);

    // Note that something is fishy with this as messages seem to have 8 bits of name extension,
    // which is wonky given each character is 6 bits.
    uint8_t nameExtensionLength = (messageSizeInBits - 272) / 6;
    name.parseExtension(streamReader, nameExtensionLength);

    return true;
}

enum NMEAMsgType NMEAVDMAidToNavigationMsg::type() const {
    return NMEA_MSG_TYPE_VDM;
}

void NMEAVDMAidToNavigationMsg::log() const {
    logger() << logDebugNMEA << "VDM Aid-To-Navigation: Repeat " << repeatIndicator << " MMSI "
             << mmsi << " " << navAidType << " " << name << " "
             << (float)longitudeTenThousandsNM / 600000 << ","
             << (float)latitudeTenThousandsNM / 600000 << eol;
}
