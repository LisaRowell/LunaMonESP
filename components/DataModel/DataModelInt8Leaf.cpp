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

#include "DataModelInt8Leaf.h"
#include "DataModelLeaf.h"

#include "etl/string.h"
#include "etl/to_string.h"

#include <stdint.h>

constexpr size_t maxStringLength = 4;

DataModelInt8Leaf::DataModelInt8Leaf(const char *name, DataModelElement *parent)
    : DataModelRetainedValueLeaf(name, parent) {
}

DataModelInt8Leaf & DataModelInt8Leaf::operator = (const int8_t value) {
    if (!hasValue() || this->value != value) {
        this->value = value;
        updated();
        etl::string<maxStringLength> valueStr;
        etl::to_string(value, valueStr);
        *this << valueStr;
    }

    return *this;
}

DataModelInt8Leaf DataModelInt8Leaf::operator ++ (int) {
    value++;
    updated();
    etl::string<maxStringLength> valueStr;
    etl::to_string(value, valueStr);
    *this << valueStr;
    return *this;
}

DataModelInt8Leaf DataModelInt8Leaf::operator -- (int) {
    value--;
    updated();
    etl::string<maxStringLength> valueStr;
    etl::to_string(value, valueStr);
    *this << valueStr;
    return *this;
}

DataModelInt8Leaf::operator int8_t() const {
    return value;
}

void DataModelInt8Leaf::sendRetainedValue(DataModelSubscriber &subscriber) {
    if (hasValue()) {
        etl::string<maxStringLength> valueStr;
        etl::to_string(value, valueStr);
        publishToSubscriber(subscriber, valueStr, true);
    }
}
