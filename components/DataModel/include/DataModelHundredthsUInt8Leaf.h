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

#ifndef DATA_MODEL_HUNDREDTHS_UINT8_LEAF_H
#define DATA_MODEL_HUNDREDTHS_UINT8_LEAF_H

#include "DataModelRetainedValueLeaf.h"

#include "HundredthsUInt8.h"

#include <stdint.h>

class DataModelNode;
class Logger;

class DataModelHundredthsUInt8Leaf : public DataModelRetainedValueLeaf {
   private:
        HundredthsUInt8 value;

        virtual void logValue(Logger &logger) override;

    public:
        DataModelHundredthsUInt8Leaf(const char *name, DataModelNode *parent);
        DataModelHundredthsUInt8Leaf & operator = (const HundredthsUInt8 &value);
        virtual void sendRetainedValue(DataModelSubscriber &subscriber) override;
};

#endif
