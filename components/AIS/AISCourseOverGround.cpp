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

#include "AISCourseOverGround.h"

#include "Logger.h"

#include "etl/bit_stream.h"

#include <stdint.h>

AISCourseOverGround::AISCourseOverGround() {
    courseCode = COURSE_OVER_GROUND_NOT_AVAILABLE;
}

AISCourseOverGround::AISCourseOverGround(etl::bit_stream_reader &streamReader) {
    courseCode = etl::read_unchecked<uint16_t>(streamReader, 12);
}

bool AISCourseOverGround::isValid() const {
    return courseCode != COURSE_OVER_GROUND_NOT_AVAILABLE;
}

Logger & operator << (Logger &logger, const AISCourseOverGround &courseOverGround) {
    if (courseOverGround.courseCode == AISCourseOverGround::COURSE_OVER_GROUND_NOT_AVAILABLE) {
        logger << "?";
    } else {
        logger << courseOverGround.courseCode / 10 << "." << courseOverGround.courseCode % 10;
    }
    logger << "deg";

    return logger;
}
