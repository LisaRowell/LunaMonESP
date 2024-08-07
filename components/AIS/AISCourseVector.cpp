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

#include "AISCourseVector.h"

#include "AISPosition.h"
#include "AISCourseOverGround.h"
#include "AISSpeedOverGround.h"

#include "Logger.h"

void AISCourseVector::set(const AISPosition &position, const AISCourseOverGround &courseOverGround,
                          const AISSpeedOverGround &speedOverGround) {
    this->position = position;
    this->courseOverGround = courseOverGround;
    this->speedOverGround = speedOverGround;
}

bool AISCourseVector::isValid() const {
    return position.isValid() && speedOverGround.isValid() &&
               (courseOverGround.isValid() || speedOverGround.isZero());
}

float AISCourseVector::distance(const AISCourseVector &other) const {
    return position.distance(other.position);
}

Logger & operator << (Logger &logger, const AISCourseVector &courseVector) {
    logger << courseVector.position << " " << courseVector.courseOverGround << " "
           << courseVector.speedOverGround;

    return logger;
}
