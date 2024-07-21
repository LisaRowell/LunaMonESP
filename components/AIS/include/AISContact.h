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

#ifndef AIS_CONTACT_H
#define AIS_CONTACT_H

#include "AISMMSI.h"
#include "AISString.h"
#include "AISShipType.h"
#include "AISNavigationAidType.h"
#include "AISDimensions.h"
#include "AISNavigationStatus.h"
#include "AISCourseVector.h"

#include <stddef.h>

class AISPosition;
class AISCourseOverGround;
class AISSpeedOverGround;

class AISContact {
    private:
        enum ContactType {
            UNKNOWN,
            SHIP,
            NAVIGATION_AID
        };
        static constexpr size_t maxNameLength = 34;

        AISMMSI mmsi;
        char nameBuffer[maxNameLength + 1];
        AISString name;
        ContactType contactType;
        union PerContactTypeUnion {
            AISShipType shipType;
            AISNavigationAidType navigationAidType;

            PerContactTypeUnion() {};
        } perContactTypeInfo;

        AISDimensions dimensions;
        AISNavigationStatus navigationStatus;
        AISCourseVector courseVector;

    public:
        AISContact(const AISMMSI &mmsi);
        void setName(const AISString &name);
        void setShipType(const AISShipType &shipType);
        void setNavigationAidType(const AISNavigationAidType navigationAidType);
        void setDimensions(const AISDimensions &dimensions);
        void setNavigationStatus(const AISNavigationStatus &navigationStatus);
        void setCourseVector(const AISPosition &position,
                             const AISCourseOverGround &courseOverGround,
                             const AISSpeedOverGround &speedOverGround);
        void dump(Logger &logger) const;
};

#endif // AIS_CONTACT_H
