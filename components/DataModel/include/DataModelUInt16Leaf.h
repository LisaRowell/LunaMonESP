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

#ifndef DATA_MODEL_UINT16_LEAF_H
#define DATA_MODEL_UINT16_LEAF_H

#include "DataModelRetainedValueLeaf.h"

#include <stdint.h>

class DataModelNode;
class Logger;

class DataModelUInt16Leaf : public DataModelRetainedValueLeaf {
   private:
        uint16_t value;

        virtual void logValue(Logger &logger) override;

    public:
        DataModelUInt16Leaf(const char *name, DataModelNode *parent);
        DataModelUInt16Leaf & operator = (const uint16_t value);
        DataModelUInt16Leaf operator ++ (int);
        DataModelUInt16Leaf operator -- (int);
        operator uint16_t() const;
        virtual void sendRetainedValue(DataModelSubscriber &subscriber) override;
};

#endif
