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

#ifndef AIS_CONTACTS_H
#define AIS_CONTACTS_H

#include "TaskObject.h"

#include "AISContact.h"
#include "AISMMSI.h"
#include "AISCourseVector.h"

#include "etl/map.h"
#include "etl/pool.h"

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

#include <stddef.h>
#include <stdint.h>

class AISPosition;
class AISCourseOverGround;
class AISSpeedOverGround;

class AISContacts : public TaskObject {
    private:
        static constexpr size_t stackSize = 8 * 1024;
        static constexpr uint32_t dumpDelayMs = 30 * 1000;
        static constexpr uint32_t lockTimeoutMs = 60 * 1000;
        static constexpr uint32_t maxContacts = 100;

        SemaphoreHandle_t contactsLock;
        etl::map<AISMMSI, AISContact *, maxContacts> contacts;
        etl::pool<AISContact, maxContacts> freeContacts;
        AISCourseVector ownCourseVector;

        virtual void task() override;
        void dumpContacts();

    public:
        AISContacts();
        void takeContactsLock();
        void releaseContactsLock();
        AISContact *findOrCreateContact(const AISMMSI &mmsi);
        void setOwnCourseVector(const AISPosition &position,
                                const AISCourseOverGround &courseOverGround,
                                const AISSpeedOverGround &speedOverGround);
        // Caller must be holding the Contacts Lock.
        void contactCourseVectorChanged(AISContact &contact);
};

#endif // AIS_CONTACTS_H
