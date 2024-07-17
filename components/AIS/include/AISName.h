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

#ifndef AIS_NAME_H
#define AIS_NAME_H

#include "LoggableItem.h"

#include "etl/string.h"
#include "etl/bit_stream.h"

#include <stddef.h>
#include <stdint.h>

class AISName : public LoggableItem {
    private:
        static constexpr size_t MAX_NAME_BASE_SIZE = 20;
        static constexpr size_t MAX_NAME_EXTENSION_SIZE = 14;
        static constexpr size_t MAX_NAME_SIZE = MAX_NAME_BASE_SIZE + MAX_NAME_EXTENSION_SIZE;

        etl::string<MAX_NAME_SIZE> name;

        char codeToChar(char sixBitCode) const;
        virtual void log(Logger &logger) const override;

    public:
        AISName();
        AISName(etl::bit_stream_reader &streamReader);
        void parse(etl::bit_stream_reader &streamReader);
        void parseExtension(etl::bit_stream_reader &streamReader, uint8_t characters);
        AISName & operator = (const AISName &other);
};

#endif // AIS_NAME_H
