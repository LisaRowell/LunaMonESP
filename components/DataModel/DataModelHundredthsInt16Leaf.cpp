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

#include "DataModelHundredthsInt16Leaf.h"
#include "DataModelLeaf.h"
#include "DataModelNode.h"

#include "HundredthsInt16.h"

#include "Logger.h"

#include "etl/string.h"
#include "etl/string_stream.h"

#include <stdint.h>

constexpr size_t maxStringLength = 9;

DataModelHundredthsInt16Leaf::DataModelHundredthsInt16Leaf(const char *name, DataModelNode *parent)
    : DataModelRetainedValueLeaf(name, parent) {
}

DataModelHundredthsInt16Leaf & DataModelHundredthsInt16Leaf::operator =
        (const HundredthsInt16 &value) {
    if (!hasValue() || this->value != value) {
        this->value = value;

        updated();

        etl::string<maxStringLength> valueStr;
        value.toString(valueStr);
        *this << valueStr;
    }

    return *this;
}

void DataModelHundredthsInt16Leaf::sendRetainedValue(DataModelSubscriber &subscriber) {
    if (hasValue()) {
        etl::string<maxStringLength> valueStr;
        value.toString(valueStr);
        publishToSubscriber(subscriber, valueStr, true);
    }
}

void DataModelHundredthsInt16Leaf::logValue(Logger &logger) {
    logger << value;
}
