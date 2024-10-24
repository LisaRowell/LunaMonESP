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

#include "DataModelBoolLeaf.h"
#include "DataModelLeaf.h"
#include "DataModelNode.h"

#include "Logger.h"

#include "etl/string.h"
#include "etl/to_string.h"

#include <stdint.h>

DataModelBoolLeaf::DataModelBoolLeaf(const char *name, DataModelNode *parent)
    : DataModelRetainedValueLeaf(name, parent),
      value(0) {
}

DataModelBoolLeaf & DataModelBoolLeaf::operator = (const bool value) {
    if (!hasValue() || this->value != value) {
        this->value = value;
        updated();
        if (value) {
            *this << (uint32_t)1;
        } else {
            *this << (uint32_t)0;
        }
    }

    return *this;
}

DataModelBoolLeaf::operator bool() const {
    return value;
}


void DataModelBoolLeaf::sendRetainedValue(DataModelSubscriber &subscriber) {
    if (hasValue()) {
        etl::string<2> valueStr;
        if (value) {
            valueStr = "1";
        } else {
            valueStr = "0";
        }
        publishToSubscriber(subscriber, valueStr, true);
    }
}

void DataModelBoolLeaf::logValue(Logger &logger) {
    logger << value;
}
