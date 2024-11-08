/*
 * This file is part of LunaMon (https://github.com/LisaRowell/LunaMonESP)
 * Copyright (C) 2021-2024 Lisa Rowell
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

#ifndef NMEA_TALKER_H
#define NMEA_TALKER_H

#include "LoggableItem.h"
#include "Logger.h"

#include "etl/string.h"
#include "etl/compare.h"

class NMEATalker : public etl::compare<NMEATalker>, public LoggableItem {
    private:
        etl::string<2> talkerCode;

    public:
        NMEATalker();
        NMEATalker(const etl::istring &talkerCode);
        const char *name() const;
        bool operator == (const NMEATalker &other) const;
        virtual void log(Logger &logger) const override;

    friend bool operator <(const NMEATalker &lhs, const NMEATalker &rhs);
    friend bool operator <=(const NMEATalker &lhs, const NMEATalker &rhs);
    friend bool operator >(const NMEATalker &lhs, const NMEATalker &rhs);
    friend bool operator >=(const NMEATalker &lhs, const NMEATalker &rhs);
};

inline bool operator <(const NMEATalker &lhs, const NMEATalker &rhs) {
    return lhs.talkerCode < rhs.talkerCode;
}

inline bool operator <=(const NMEATalker &lhs, const NMEATalker &rhs) {
    return lhs.talkerCode <= rhs.talkerCode;
}

inline bool operator >(const NMEATalker &lhs, const NMEATalker &rhs) {
    return lhs.talkerCode > rhs.talkerCode;
}

inline bool operator >=(const NMEATalker &lhs, const NMEATalker &rhs) {
    return lhs.talkerCode >= rhs.talkerCode;
}

#endif
