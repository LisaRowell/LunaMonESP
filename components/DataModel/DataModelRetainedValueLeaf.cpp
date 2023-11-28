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

#include "DataModelRetainedValueLeaf.h"
#include "DataModelLeaf.h"

#include <etl/string.h>

#include <stdint.h>

uint16_t DataModelRetainedValueLeaf::retainedValues = 0;

uint16_t DataModelRetainedValueLeaf::retainedValueCount() {
    return retainedValues;
}

DataModelRetainedValueLeaf::DataModelRetainedValueLeaf(const char *name, DataModelElement *parent)
    : DataModelLeaf(name, parent), hasBeenSet(false) {
}

bool DataModelRetainedValueLeaf::subscribe(DataModelSubscriber &subscriber, uint32_t cookie) {
    if (!DataModelLeaf::subscribe(subscriber, cookie)) {
        return false;
    }

    sendRetainedValue(subscriber);

    return true;
}

void DataModelRetainedValueLeaf::updated() {
    if (!hasBeenSet) {
        retainedValues++;
        hasBeenSet = true;
    }
}

void DataModelRetainedValueLeaf::removeValue() {
    if (hasBeenSet) {
        etl::string<1> emptyStr;

        *this << emptyStr;
        hasBeenSet = false;
    }
}

bool DataModelRetainedValueLeaf::hasValue() const {
    return hasBeenSet;
}
