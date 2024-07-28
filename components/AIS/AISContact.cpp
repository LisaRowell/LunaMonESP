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

#include "AISContact.h"

#include "AISMMSI.h"
#include "AISString.h"
#include "AISShipType.h"
#include "AISDimensions.h"
#include "AISNavigationStatus.h"
#include "AISCourseVector.h"
#include "AISPosition.h"
#include "AISCourseOverGround.h"
#include "AISSpeedOverGround.h"

#include "Logger.h"

AISContact::AISContact(const AISMMSI &mmsi)
    : mmsi(mmsi),
      name(nameBuffer, maxNameLength),
      contactType(UNKNOWN),
      distanceKnown(false) {
    this->name = "Unknown name";
}

void AISContact::setName(const AISString &name) {
    if (name.isEmpty()) {
        this->name = "Unknown name";
    } else {
        this->name = name;
    }
}

void AISContact::setShipType(const AISShipType &shipType) {
    if (contactType == NAVIGATION_AID) {
        logger() << logWarnAIS << "Contact type for MMSI " << mmsi
                 << " changed from navigation aid to ship" << eol;
    }
    contactType = SHIP;
    perContactTypeInfo.shipType = shipType;
}

void AISContact::setNavigationAidType(const AISNavigationAidType navigationAidType) {
    if (contactType == SHIP) {
        logger() << logWarnAIS << "Contact type for MMSI " << mmsi
                 << " changed from ship to navigation aid" << eol;
    }
    contactType = NAVIGATION_AID;
    perContactTypeInfo.navigationAidType = navigationAidType;
}

void AISContact::setDimensions(const AISDimensions &dimensions) {
    this->dimensions = dimensions;
}

void AISContact::setNavigationStatus(const AISNavigationStatus &navigationStatus) {
    this->navigationStatus = navigationStatus;
}

void AISContact::setCourseVector(const AISPosition &position,
                                 const AISCourseOverGround &courseOverGround,
                                 const AISSpeedOverGround &speedOverGround) {
    courseVector.set(position, courseOverGround, speedOverGround);
}

void AISContact::updateDistance(const AISCourseVector &ownCourseVector) {
    distance = courseVector.distance(ownCourseVector);
    distanceKnown = true;
}

void AISContact::dump(Logger &logger) const {
    logger << "    " << mmsi << " " << name << " ";

    switch (contactType) {
        case UNKNOWN:
            logger << "?";
            break;
        case SHIP:
            logger << perContactTypeInfo.shipType;
            break;
        case NAVIGATION_AID:
            logger << perContactTypeInfo.navigationAidType;
            break;
    }

    logger << " " << dimensions << " "
           << navigationStatus << " " << courseVector;

    logger << " ";
    if (distanceKnown) {
        logger << distance;
    } else {
        logger << "?";
    }
    logger << "NM" << eol;
}
