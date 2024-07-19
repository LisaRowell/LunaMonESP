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

#include "AISMessage.h"
#include "AISContacts.h"
#include "AISMsgType.h"
#include "AISMMSI.h"
#include "AISString.h"
#include "AISShipType.h"
#include "AISShipDimensions.h"
#include "AISEPFDFixType.h"

#include "Logger.h"

#include "etl/bit_stream.h"

#include <stddef.h>
#include <stdint.h>

bool AISMessage::parse(etl::bit_stream_reader &streamReader, size_t messageSizeInBits,
                       bool ownShip, AISContacts &aisContacts) {
    if (messageSizeInBits < 6) {
        logger() << logWarnAIS << "AIS message that's too small to be valid ("
                 << messageSizeInBits << " bits)" << eol;
        return false;
    }

    msgType.parse(streamReader);

    switch (msgType) {
        case AISMsgType::STATIC_AND_VOYAGE_DATA:
            return parseStaticAndVoyageRelatedData(streamReader, messageSizeInBits, ownShip,
                                                   aisContacts);

        case AISMsgType::STATIC_DATA_REPORT:
            return parseStaticDataReport(streamReader, messageSizeInBits, ownShip, aisContacts);

        default:
            logger() << logNotifyAIS << "Ignoring " << messageSizeInBits << " bit AIS "
                     << msgType << " message" << eol;
            return false;
    }
}

void AISMessage::log(const char *nmeaMsgTypeName) const {
    logger() << nmeaMsgTypeName << " AIS Message" << eol;
}

bool AISMessage::parseStaticAndVoyageRelatedData(etl::bit_stream_reader &streamReader,
                                                 size_t messageSizeInBits, bool ownShip,
                                                 AISContacts &aisContacts) {
    logger() << logDebugAIS << "Parsing " << messageSizeInBits
             << " AIS Static and Voyage Related Data" << eol;

    // While the message should be 424 bits, it's not uncommon for them to be truncated to 422 or
    // even 420 bits.
    if (messageSizeInBits != 424 && messageSizeInBits != 422 && messageSizeInBits != 420) {
        logger() << logWarnAIS << "Static and Voyage Related Data Msg with bad length ("
                 << messageSizeInBits << ")" << eol;
        return false;
    }

    [[maybe_unused]] uint8_t repeatIndicator = etl::read_unchecked<uint8_t>(streamReader, 2);
    AISMMSI mmsi(streamReader);
    [[maybe_unused]] uint8_t aisVersion = etl::read_unchecked<uint8_t>(streamReader, 2);
    [[maybe_unused]] uint32_t imoNumber = etl::read_unchecked<uint32_t>(streamReader, 30);
    char callSignBuffer[7 + 1];
    AISString callSign(callSignBuffer, 7, streamReader);
    char vesselNameBuffer[20 + 1];
    AISString vesselName(vesselNameBuffer, 20, streamReader);
    vesselName.removeTrailingBlanks();
    AISShipType shipType(streamReader);
    AISShipDimensions dimensions(streamReader);
    AISEPFDFixType epfdFixType(streamReader);
    [[maybe_unused]] uint8_t etaMonth = etl::read_unchecked<uint8_t>(streamReader, 4);
    [[maybe_unused]] uint8_t etaDay = etl::read_unchecked<uint8_t>(streamReader, 5);
    [[maybe_unused]] uint8_t etaHour = etl::read_unchecked<uint8_t>(streamReader, 5);
    [[maybe_unused]] uint8_t etaMinute = etl::read_unchecked<uint8_t>(streamReader, 6);
    uint8_t draughtTensM = etl::read_unchecked<uint8_t>(streamReader, 8);
    [[maybe_unused]] uint16_t draughtM = draughtTensM * 10;
    char destinationBuffer[20 + 1];
    AISString destination(destinationBuffer, 20);
    if (messageSizeInBits > 420) {
        destination.append(20, streamReader);
    } else {
        destination.append(18, streamReader);
    }
    destination.removeTrailingBlanks();
    if (messageSizeInBits > 422) {
        [[maybe_unused]] bool dataTerminalReady = etl::read_unchecked<bool>(streamReader);
    }

    Logger &currentLogger = logger();
    currentLogger << logDebugAIS << "Static and Voyage Related Data MMSI: " << mmsi << " name: "
                  << vesselName << " ship type: " << shipType << " call sign: " << callSign << " "
                  << dimensions << " Fix: " << epfdFixType;
    if (ownShip) {
        currentLogger << " own ship";
    }
    currentLogger << eol;

    if (!ownShip) {
        aisContacts.takeContactsLock();
        AISContact *contact = aisContacts.findOrCreateContact(mmsi);
        if (contact == nullptr) {
            createContactError("Static and Voyage Related Data", mmsi);
        } else {
            contact->setName(vesselName);
            contact->setShipType(shipType);
            if (dimensions.isSet()) {
                contact->setDimensions(dimensions);
            }
        }
        aisContacts.releaseContactsLock();
    }

    return true;
}

bool AISMessage::parseStaticDataReport(etl::bit_stream_reader &streamReader,
                                       size_t messageSizeInBits, bool ownShip,
                                       AISContacts &aisContacts) {
    logger() << logDebugAIS << "Parsing " << messageSizeInBits << " AIS Static Data Report" << eol;

    // Part A and Part B messages can differ in length. Make sure we have enough of a message to
    // read up to and including the part number.
    if (messageSizeInBits < 40) {
        logger() << logWarnAIS << "Static Data Report Msg with bad length ("
                 << messageSizeInBits << ")" << eol;
        return false;
    }

    uint8_t repeatIndicator = etl::read_unchecked<uint8_t>(streamReader, 2);
    AISMMSI mmsi(streamReader);

    uint8_t partNumber = etl::read_unchecked<uint8_t>(streamReader, 2);
    switch (partNumber) {
        case 0:
            parseStaticDataReportPartA(streamReader, messageSizeInBits, ownShip, aisContacts,
                                       repeatIndicator, mmsi);
            return true;

        case 1:
            parseStaticDataReportPartB(streamReader, messageSizeInBits, ownShip, aisContacts,
                                       repeatIndicator, mmsi);
            return true;

        default:
            logger() << logWarnAIS << "Static Data Report Msg with bad part number (" << partNumber
                     << ")" << eol;
            return false;
    }

    return true;
}

void AISMessage::parseStaticDataReportPartA(etl::bit_stream_reader &streamReader,
                                            size_t messageSizeInBits, bool ownShip,
                                            AISContacts &aisContacts, uint8_t repeatIndicator,
                                            AISMMSI &mmsi) {
    if (messageSizeInBits != 160 && messageSizeInBits != 168) {
        logger() << logWarnAIS << "Static Data Report Msg Part A with bad length ("
                 << messageSizeInBits << ")" << eol;
        return;
    }

    char vesselNameBuffer[20 + 1];
    AISString vesselName(vesselNameBuffer, 20, streamReader);
    vesselName.removeTrailingBlanks();

    Logger &currentLogger = logger();
    currentLogger << logDebugAIS << "Static Data Report Pt A MMSI: " << mmsi << " name: "
                  << vesselName;
    if (ownShip) {
        currentLogger << " own ship";
    }
    currentLogger << eol;

    if (!ownShip) {
        aisContacts.takeContactsLock();
        AISContact *contact = aisContacts.findOrCreateContact(mmsi);
        if (contact == nullptr) {
            createContactError("Static Data Report", mmsi);
        } else {
            contact->setName(vesselName);
        }
        aisContacts.releaseContactsLock();
    }
}

void AISMessage::parseStaticDataReportPartB(etl::bit_stream_reader &streamReader,
                                            size_t messageSizeInBits, bool ownShip,
                                            AISContacts &aisContacts, uint8_t repeatIndicator,
                                            AISMMSI &mmsi) {
    if (messageSizeInBits != 168) {
        logger() << logWarnAIS << "Static Data Report Msg Part B with bad length ("
                 << messageSizeInBits << ")" << eol;
        return;
    }

    AISShipType shipType(streamReader);
    char vendorIDBuffer[3 + 1];
    AISString vendorID(vendorIDBuffer, 3, streamReader);
    [[maybe_unused]] uint8_t unitModelCode = etl::read_unchecked<uint8_t>(streamReader, 4);
    [[maybe_unused]] uint32_t serialNumber = etl::read_unchecked<uint32_t>(streamReader, 20);
    char callSignBuffer[7 + 1];
    AISString callSign(callSignBuffer, 7, streamReader);

    // The rest of the message has two different formats based on whether or not the report is for
    // a mothership or for an auxilliary craft
    AISMMSI mothershipMMSI;
    AISShipDimensions dimensions;
    if (!mmsi.isAuxiliaryCraft()) {
        dimensions.set(streamReader);
    } else {
        mothershipMMSI.set(streamReader);
    }
    Logger &currentLogger = logger();
    currentLogger << logDebugAIS << "Static Data Report Pt B MMSI: " << mmsi << " ship type: "
                  << shipType << " vendor ID: " << vendorID << " call sign: " << callSign;
    if (dimensions.isSet()) {
        currentLogger << " " << dimensions;
    }
    if (mothershipMMSI.isSet()) {
        currentLogger << " mothership: " << mothershipMMSI;
    }
    if (ownShip) {
        currentLogger << " own ship";
    }
    currentLogger << eol;

    if (!ownShip) {
        aisContacts.takeContactsLock();
        AISContact *contact = aisContacts.findOrCreateContact(mmsi);
        if (contact == nullptr) {
            createContactError("Static Data Report", mmsi);
        } else {
            contact->setShipType(shipType);
            if (dimensions.isSet()) {
                contact->setDimensions(dimensions);
            }
        }
        aisContacts.releaseContactsLock();
    }
}

void AISMessage::createContactError(const char *messageTypeName, AISMMSI &mmsi) {
    logger() << logWarnAIS << "Failed to create contact from " << messageTypeName
             << " message from mmsi " << mmsi << ". Table contact full." << eol;
}