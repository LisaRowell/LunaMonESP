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

#include "AISContacts.h"

#include "Error.h"

#include <freertos/semphr.h>

#include <stddef.h>

AISContacts::AISContacts() : TaskObject("AIS Contacts", LOGGER_LEVEL_DEBUG, stackSize) {
    if ((contactsLock = xSemaphoreCreateMutex()) == nullptr) {
        logger << logErrorAIS << "Failed to create contactsLock mutex" << eol;
        errorExit();
    }
}

void AISContacts::task() {
    while (1) {
        if (logger.debugEnabled(LOGGER_MODULE_AIS)) {
            dumpContacts();
        }

        vTaskDelay(pdMS_TO_TICKS(dumpDelayMs));
    }
}

void AISContacts::dumpContacts() {
    logger << logDebugAIS << "AIS Contacts:" << eol;

    takeContactsLock();
    for (auto const &contactIterator : contacts) {
        const AISContact &contact = *contactIterator.second;
        contact.dump(logger);
    }
    releaseContactsLock();
}

void AISContacts::takeContactsLock() {
    if (xSemaphoreTake(contactsLock, pdMS_TO_TICKS(lockTimeoutMs)) != pdTRUE) {
        taskLogger() << logErrorAIS << "Failed to get contacts lock mutex" << eol;
        errorExit();
    }
}

void AISContacts::releaseContactsLock() {
    xSemaphoreGive(contactsLock);
}

// Must be called with the contacts lock taken
AISContact *AISContacts::findOrCreateContact(AISMMSI &mmsi) {
    auto contactIterator = contacts.find(mmsi);
    if (contactIterator == contacts.end()) {
        AISContact *contact = new (freeContacts.allocate()) AISContact(mmsi);
        if (contact != nullptr) {
            logger << logDebugAIS << "Created new contact for mmsi " << mmsi << eol;
            contacts[mmsi] = contact;
            return contact;
        } else {
            logger << logWarnAIS << "Failed to create contact for mmsi " << mmsi
                   << ", maximum contacts reached" << eol;
            return nullptr;
        }
    } else {
        return contactIterator->second;
    }
}

void AISContacts::setOwnCourseVector(AISPosition &position, AISCourseOverGround &courseOverGround,
                                     AISSpeedOverGround &speedOverGround) {
    ownCourseVector.set(position, courseOverGround, speedOverGround);
}
