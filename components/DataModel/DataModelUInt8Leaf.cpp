/*
 * This file is part of LunaMon (https://github.com/LisaRowell/LunaMon)
 * Copyright (C) 2021-2023 Lisa Rowell
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

#include "DataModelUInt8Leaf.h"
#include "DataModelLeaf.h"

#include "etl/string.h"
#include "etl/to_string.h"

#include <stdint.h>

DataModelUInt8Leaf::DataModelUInt8Leaf(const char *name, DataModelElement *parent)
    : DataModelRetainedValueLeaf(name, parent) {
}

DataModelUInt8Leaf & DataModelUInt8Leaf::operator = (const uint8_t value) {
    if (!hasValue() || this->value != value) {
        this->value = value;
        updated();
        *this << (uint32_t)value;
    }

    return *this;
}

DataModelUInt8Leaf DataModelUInt8Leaf::operator ++ (int) {
    value++;
    updated();
    *this << (uint32_t)value;
    return *this;
}

DataModelUInt8Leaf DataModelUInt8Leaf::operator -- (int) {
    value--;
    updated();
    *this << (uint32_t)value;
    return *this;
}

DataModelUInt8Leaf::operator uint8_t() const {
    return value;
}


void DataModelUInt8Leaf::sendRetainedValue(DataModelSubscriber &subscriber) {
    if (hasValue()) {
        etl::string<3> valueStr;
        etl::to_string(value, valueStr);
        publishToSubscriber(subscriber, valueStr, true);
    }
}
