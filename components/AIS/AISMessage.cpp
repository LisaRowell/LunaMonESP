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
#include "AISName.h"
#include "AISShipType.h"
#include "AISShipDimensions.h"
#include "AISString.h"

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
        case AISMsgType::STATIC_DATA_REPORT:
            parseStaticDataReport(streamReader, messageSizeInBits, ownShip, aisContacts);
            return true;

        default:
            logger() << logNotifyAIS << "Ignoring " << messageSizeInBits << " bit AIS "
                     << msgType << " message" << eol;
            return false;
    }
}

void AISMessage::log(const char *nmeaMsgTypeName) const {
    logger() << nmeaMsgTypeName << " AIS Message" << eol;
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

    AISName vesselName(streamReader);

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