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

#include "DataModelUInt32Leaf.h"
#include "DataModelLeaf.h"
#include "DataModelNode.h"

#include "Logger.h"

#include "etl/string.h"
#include "etl/to_string.h"

#include <stdint.h>

DataModelUInt32Leaf::DataModelUInt32Leaf(const char *name, DataModelNode *parent)
    : DataModelRetainedValueLeaf(name, parent),
      value(0) {
}

DataModelUInt32Leaf & DataModelUInt32Leaf::operator = (const uint32_t value) {
    if (!hasValue() || this->value != value) {
        this->value = value;
        updated();
        *this << value;
    }

    return *this;
}

DataModelUInt32Leaf DataModelUInt32Leaf::operator ++ (int) {
    value++;
    updated();
    *this << value;
    return *this;
}

DataModelUInt32Leaf DataModelUInt32Leaf::operator -- (int) {
    value--;
    updated();
    *this << value;
    return *this;
}

DataModelUInt32Leaf::operator uint32_t() const {
    return value;
}

void DataModelUInt32Leaf::sendRetainedValue(DataModelSubscriber &subscriber) {
    if (hasValue()) {
        etl::string<12> valueStr;
        etl::to_string(value, valueStr);
        publishToSubscriber(subscriber, valueStr, true);
    }
}

void DataModelUInt32Leaf::logValue(Logger &logger) {
    logger << value;
}
