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

#include "Logger.h"

AISContact::AISContact(AISMMSI &mmsi) : mmsi(mmsi) {
}

void AISContact::setName(AISName &name) {
    this->name = name;
}

void AISContact::setShipType(AISShipType &shipType) {
    this->shipType = shipType;
}

void AISContact::setDimensions(AISShipDimensions &dimensions) {
    this->dimensions = dimensions;
}

void AISContact::dump(Logger &logger) const {
    logger << "    " << mmsi << " " << name << " " << shipType << " " << dimensions << eol;
}
