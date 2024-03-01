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

#include "DataModelHundredthsUInt8Leaf.h"
#include "DataModelLeaf.h"
#include "DataModelNode.h"

#include "HundredthsUInt8.h"

#include "etl/string.h"
#include "etl/string_stream.h"

#include <stdint.h>

constexpr size_t maxStringLength = 6;

DataModelHundredthsUInt8Leaf::DataModelHundredthsUInt8Leaf(const char *name, DataModelNode *parent)
    : DataModelRetainedValueLeaf(name, parent) {
}

DataModelHundredthsUInt8Leaf & DataModelHundredthsUInt8Leaf::operator =
        (const HundredthsUInt8 &value) {
    if (!hasValue() || this->value != value) {
        this->value = value;

        updated();

        etl::string<maxStringLength> valueStr;
        value.toString(valueStr);
        *this << valueStr;
    }

    return *this;
}

void DataModelHundredthsUInt8Leaf::sendRetainedValue(DataModelSubscriber &subscriber) {
    if (hasValue()) {
        etl::string<maxStringLength> valueStr;
        value.toString(valueStr);
        publishToSubscriber(subscriber, valueStr, true);
    }
}
