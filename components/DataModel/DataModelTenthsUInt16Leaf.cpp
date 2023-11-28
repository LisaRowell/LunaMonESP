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

#include "DataModelTenthsUInt16Leaf.h"
#include "DataModelLeaf.h"

#include <etl/string.h>
#include <etl/string_stream.h>

#include <stdint.h>

constexpr size_t maxStringLength = 7;

DataModelTenthsUInt16Leaf::DataModelTenthsUInt16Leaf(const char *name, DataModelElement *parent)
    : DataModelRetainedValueLeaf(name, parent) {
}

void DataModelTenthsUInt16Leaf::set(uint16_t wholeNumber, uint8_t tenths) {
    if (!hasValue() || this->wholeNumber != wholeNumber || this->tenths != tenths) {
        this->wholeNumber = wholeNumber;
        this->tenths = tenths;
        updated();
        etl::string<maxStringLength> valueStr;
        etl::string_stream valueStrStream(valueStr);
        valueStrStream << wholeNumber << "." << tenths;
        *this << valueStr;
    }
}

void DataModelTenthsUInt16Leaf::sendRetainedValue(DataModelSubscriber &subscriber) {
    if (hasValue()) {
        etl::string<maxStringLength> valueStr;
        etl::string_stream valueStrStream(valueStr);
        valueStrStream << wholeNumber << "." << tenths;
        publishToSubscriber(subscriber, valueStr, true);
    }
}
