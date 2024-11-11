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
#include "AISDimensions.h"
#include "AISEPFDFixType.h"
#include "AISNavigationStatus.h"
#include "AISRateOfTurn.h"
#include "AISSpeedOverGround.h"
#include "AISCourseOverGround.h"
#include "AISNavigationAidType.h"

#include "Logger.h"

#include "etl/bit_stream.h"
#include "etl/string.h"
#include "etl/string_stream.h"

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
        case AISMsgType::POS_REPORT_CLASS_A:
        case AISMsgType::POS_REPORT_CLASS_A_ASS_SCHED:
        case AISMsgType::POS_REPORT_CLASS_A_RESPONSE:
            return parseCommonNavigationBlock(streamReader, messageSizeInBits, ownShip,
                                              aisContacts);

        case AISMsgType::BASE_STATION_REPORT:
            return parseBaseStationReport(streamReader, messageSizeInBits, ownShip, aisContacts);

        case AISMsgType::STATIC_AND_VOYAGE_DATA:
            return parseStaticAndVoyageRelatedData(streamReader, messageSizeInBits, ownShip,
                                                   aisContacts);

        case AISMsgType::STANDARD_CLASS_B_POS_REPORT:
            return parseStandardClassBPositionReport(streamReader, messageSizeInBits, ownShip,
                                                     aisContacts);

        case AISMsgType::AID_TO_NAVIGATION_REPORT:
            return parseAidToNavigationReport(streamReader, messageSizeInBits, ownShip,
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
    logger() << nmeaMsgTypeName << " AIS " << msgType << " message" << eol;
}

bool AISMessage::parseCommonNavigationBlock(etl::bit_stream_reader &streamReader,
                                            size_t messageSizeInBits, bool ownShip,
                                            AISContacts &aisContacts) {
    if (messageSizeInBits != 168) {
        logger() << logWarnAIS << msgType << " Msg with bad length (" << messageSizeInBits << ")"
                 << eol;
        return false;
    }

    [[maybe_unused]] uint8_t repeatIndicator = etl::read_unchecked<uint8_t>(streamReader, 2);
    AISMMSI mmsi(streamReader);
    AISNavigationStatus navigationStatus(streamReader);
    AISRateOfTurn rateOfTurn(streamReader);
    AISSpeedOverGround speedOverGround(streamReader);
    [[maybe_unused]] bool positionAccuracy = etl::read_unchecked<bool>(streamReader);
    AISPosition position(streamReader);
    AISCourseOverGround courseOverGround(streamReader);
    [[maybe_unused]] uint16_t heading = etl::read_unchecked<uint16_t>(streamReader, 9);
    [[maybe_unused]] uint8_t timeStampSeconds = read_unchecked<uint8_t>(streamReader, 6);
    [[maybe_unused]] uint8_t maneuverIndicator = read_unchecked<uint8_t>(streamReader, 2);
    streamReader.skip(3);
    [[maybe_unused]] bool raimFlag = etl::read_unchecked<bool>(streamReader);
    [[maybe_unused]] uint32_t radioStatus = etl::read_unchecked<uint32_t>(streamReader, 19);

    Logger &currentLogger = logger();
    currentLogger << logDebugAIS <<  msgType << " MMSI: " << mmsi << " NavStatus: "
                  << navigationStatus << " " << position << " " << courseOverGround << " "
                  << speedOverGround << " RateOfTurn " << rateOfTurn;
    if (ownShip) {
        currentLogger << " own ship";
    }
    currentLogger << eol;

    if (ownShip) {
        aisContacts.setOwnCourseVector(position, courseOverGround, speedOverGround);
    } else {
        aisContacts.takeContactsLock();
        AISContact *contact = aisContacts.findOrCreateContact(mmsi);
        if (contact == nullptr) {
            createContactError(mmsi);
        } else {
            contact->setNavigationStatus(navigationStatus);
            contact->setCourseVector(position, courseOverGround, speedOverGround);
            aisContacts.contactCourseVectorChanged(*contact);
        }
        aisContacts.releaseContactsLock();
    }

    return true;
}

bool AISMessage::parseBaseStationReport(etl::bit_stream_reader &streamReader,
                                        size_t messageSizeInBits, bool ownShip,
                                        AISContacts &aisContacts) {
    if (messageSizeInBits != 168) {
        logger() << logWarnAIS << "Base Station Report with bad length ("
                 << messageSizeInBits << ")" << eol;
        return false;
    }

    [[maybe_unused]] uint8_t repeatIndicator = etl::read_unchecked<uint8_t>(streamReader, 2);
    AISMMSI mmsi(streamReader);
    uint16_t year = etl::read_unchecked<uint16_t>(streamReader, 14);
    uint8_t month = etl::read_unchecked<uint8_t>(streamReader, 4);
    uint8_t day = etl::read_unchecked<uint8_t>(streamReader, 5);
    uint8_t hour = etl::read_unchecked<uint8_t>(streamReader, 5);
    uint8_t minute = etl::read_unchecked<uint8_t>(streamReader, 6);
    uint8_t second = etl::read_unchecked<uint8_t>(streamReader, 6);
    [[maybe_unused]] bool positionAccuracy = etl::read_unchecked<bool>(streamReader);
    AISPosition position(streamReader);
    AISEPFDFixType epfdFixType(streamReader);
    streamReader.skip(10);
    [[maybe_unused]] bool raimFlag = etl::read_unchecked<bool>(streamReader);

    etl::string<9> timeStr;
    etl::string_stream timeStrStream(timeStr);
    timeStrStream << etl::setfill('0')<< etl::setw(2) << hour << etl::setw(0) << ":" << etl::setw(2)
                  << minute << etl::setw(0) << ":" << etl::setw(2) << second;

    Logger &currentLogger = logger();
    currentLogger << logDebugAIS <<  msgType << " MMSI: " << mmsi << " " << month << "/" << day
                  << "/" << year << " " << timeStr << " " << position << " " << " Fix: "
                  << epfdFixType;
    if (ownShip) {
        currentLogger << " own ship";
    }
    currentLogger << eol;

    // Since it's a base station, it's position course is undefined and it's speed better be zero.
    AISCourseOverGround courseOverGround;
    AISSpeedOverGround speedOverGround(0);

    if (ownShip) {
        aisContacts.setOwnCourseVector(position, courseOverGround, speedOverGround);
    } else {
        aisContacts.takeContactsLock();
        AISContact *contact = aisContacts.findOrCreateContact(mmsi);
        if (contact == nullptr) {
            createContactError(mmsi);
        } else {
            contact->setCourseVector(position, courseOverGround, speedOverGround);
            aisContacts.contactCourseVectorChanged(*contact);
        }
        aisContacts.releaseContactsLock();
    }

    return true;
}

bool AISMessage::parseStaticAndVoyageRelatedData(etl::bit_stream_reader &streamReader,
                                                 size_t messageSizeInBits, bool ownShip,
                                                 AISContacts &aisContacts) {
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
    AISDimensions dimensions(streamReader);
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
            createContactError(mmsi);
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

bool AISMessage::parseStandardClassBPositionReport(etl::bit_stream_reader &streamReader,
                                                   size_t messageSizeInBits, bool ownShip,
                                                   AISContacts &aisContacts) {
    if (messageSizeInBits != 168) {
        logger() << logWarnAIS << "Standard Class B Position Report Msg with bad length ("
                 << messageSizeInBits << ")" << eol;
        return false;
    }

    [[maybe_unused]] uint8_t repeatIndicator = etl::read_unchecked<uint8_t>(streamReader, 2);
    AISMMSI mmsi(streamReader);
    streamReader.skip(8);
    AISSpeedOverGround speedOverGround(streamReader);
    [[maybe_unused]] bool positionAccuracy = etl::read_unchecked<bool>(streamReader);
    AISPosition position(streamReader);
    AISCourseOverGround courseOverGround(streamReader);
    [[maybe_unused]] uint16_t heading = etl::read_unchecked<uint16_t>(streamReader, 9);
    [[maybe_unused]] uint8_t timeStampSeconds = read_unchecked<uint8_t>(streamReader, 6);
    streamReader.skip(2);
    [[maybe_unused]] bool csUnit = etl::read_unchecked<bool>(streamReader);
    [[maybe_unused]] bool hasDisplay = etl::read_unchecked<bool>(streamReader);
    [[maybe_unused]] bool hasDSC = etl::read_unchecked<bool>(streamReader);
    [[maybe_unused]] bool bandIsChangable = etl::read_unchecked<bool>(streamReader);
    [[maybe_unused]] bool supportsMessage22 = etl::read_unchecked<bool>(streamReader);
    [[maybe_unused]] bool assignedMode = etl::read_unchecked<bool>(streamReader);
    [[maybe_unused]] bool raimFlag = etl::read_unchecked<bool>(streamReader);
    [[maybe_unused]] uint32_t radioStatus = etl::read_unchecked<uint32_t>(streamReader, 20);

    Logger &currentLogger = logger();
    currentLogger << logDebugAIS <<  msgType << " MMSI: " << mmsi << " " << position << " "
                  << courseOverGround << " " << speedOverGround;
    if (ownShip) {
        currentLogger << " own ship";
    }
    currentLogger << eol;

    if (ownShip) {
        aisContacts.setOwnCourseVector(position, courseOverGround, speedOverGround);
    } else {
        aisContacts.takeContactsLock();
        AISContact *contact = aisContacts.findOrCreateContact(mmsi);
        if (contact == nullptr) {
            createContactError(mmsi);
        } else {
            contact->setCourseVector(position, courseOverGround, speedOverGround);
            aisContacts.contactCourseVectorChanged(*contact);
        }
        aisContacts.releaseContactsLock();
    }

    return true;
}

bool AISMessage::parseAidToNavigationReport(etl::bit_stream_reader &streamReader,
                                            size_t messageSizeInBits, bool ownShip,
                                            AISContacts &aisContacts) {
    if (messageSizeInBits < 272 || messageSizeInBits > 360) {
        logger() << logWarnAIS << "Aid to Navigation Report with bad length ("
                 << messageSizeInBits << ")" << eol;
        return false;
    }
    [[maybe_unused]] uint8_t repeatIndicator = etl::read_unchecked<uint8_t>(streamReader, 2);
    AISMMSI mmsi = etl::read_unchecked<uint32_t>(streamReader, 30);
    AISNavigationAidType navigationAidType(streamReader);
    char nameBuffer[20 + 14 + 1];
    AISString name(nameBuffer, 20, streamReader);
    [[maybe_unused]] bool positionAccuracy = etl::read_unchecked<bool>(streamReader);
    AISPosition position(streamReader);
    AISDimensions dimensions(streamReader);
    [[maybe_unused]] AISEPFDFixType epfdFixType(streamReader);
    [[maybe_unused]] uint8_t timeStampSeconds = read_unchecked<uint8_t>(streamReader, 6);
    [[maybe_unused]] bool offPosition = etl::read_unchecked<bool>(streamReader);
    streamReader.skip(8);
    [[maybe_unused]] bool raimFlag = etl::read_unchecked<bool>(streamReader);
    [[maybe_unused]] bool virtualAid = etl::read_unchecked<bool>(streamReader);
    [[maybe_unused]] bool assignedMode = etl::read_unchecked<bool>(streamReader);
    streamReader.skip(1);

    // Note that something is fishy with this as messages seem to have 8 bits of name extension,
    // which is wonky given each character is 6 bits.
    uint8_t nameExtensionLength = (messageSizeInBits - 272) / 6;
    name.append(nameExtensionLength, streamReader);
    name.removeTrailingBlanks();

    Logger &currentLogger = logger();
    currentLogger << logDebugAIS << "Aid to Navigation Report MMSI: " << mmsi << " name: " << name
                  << " aid type: " << navigationAidType << " " << dimensions << " " << position;
    if (ownShip) {
        currentLogger << " own ship";
    }
    currentLogger << eol;

    // Since this is a report for objects that, at least in theory, don't move, it doesn't include
    // course or speed information, the former being undefined and the later inferred to be 0.
    AISCourseOverGround courseOverGround;
    AISSpeedOverGround speedOverGround(0);

    if (ownShip) {
        aisContacts.setOwnCourseVector(position, courseOverGround, speedOverGround);
    } else {
        aisContacts.takeContactsLock();
        AISContact *contact = aisContacts.findOrCreateContact(mmsi);
        if (contact == nullptr) {
            createContactError(mmsi);
        } else {
            contact->setName(name);
            contact->setNavigationAidType(navigationAidType);
            contact->setDimensions(dimensions);
            contact->setCourseVector(position, courseOverGround, speedOverGround);
            aisContacts.contactCourseVectorChanged(*contact);
        }
        aisContacts.releaseContactsLock();
    }

    return true;
}

bool AISMessage::parseStaticDataReport(etl::bit_stream_reader &streamReader,
                                       size_t messageSizeInBits, bool ownShip,
                                       AISContacts &aisContacts) {
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
            createContactError(mmsi);
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
    AISDimensions dimensions;
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
            createContactError(mmsi);
        } else {
            contact->setShipType(shipType);
            if (dimensions.isSet()) {
                contact->setDimensions(dimensions);
            }
        }
        aisContacts.releaseContactsLock();
    }
}

void AISMessage::createContactError(AISMMSI &mmsi) {
    logger() << logWarnAIS << "Failed to create contact from " << msgType << " message from mmsi "
             << mmsi << ". Table contact full." << eol;
}
