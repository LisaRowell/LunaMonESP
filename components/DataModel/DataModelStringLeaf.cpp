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

#include "DataModelStringLeaf.h"
#include "DataModelNode.h"

#include "etl/string.h"

#include <stddef.h>

DataModelStringLeaf::DataModelStringLeaf(const char *name, DataModelNode *parent,
                                         etl::istring &buffer)
    : DataModelRetainedValueLeaf(name, parent), value(buffer) {
}

DataModelStringLeaf & DataModelStringLeaf::operator = (const etl::istring &newString) {
    if (!hasValue() || value.compare(newString) != 0) {
        value = newString;
        updated();
        *this << value;
    }

    return *this;
}

DataModelStringLeaf & DataModelStringLeaf::operator = (const char *newString) {
    if (!hasValue() || value.compare(newString) != 0) {
        value = newString;
        updated();
        *this << value;
    }

    return *this;
}

DataModelStringLeaf & DataModelStringLeaf::operator = (const DataModelStringLeaf &otherLeaf) {
    if (!hasValue() || value.compare(otherLeaf.value) != 0) {
        this->value = otherLeaf.value;
        updated();
        *this << value;
    }

    return *this;
}

DataModelStringLeaf::operator const char * () const {
    return value.c_str();
}

int DataModelStringLeaf::compare(const etl::istring &otherString) const {
    return value.compare(otherString);
}

void DataModelStringLeaf::sendRetainedValue(DataModelSubscriber &subscriber) {
    if (hasValue()) {
        publishToSubscriber(subscriber, value, true);
    }
}

bool DataModelStringLeaf::isEmptyStr() const {
    return hasValue() && value.empty();
}
