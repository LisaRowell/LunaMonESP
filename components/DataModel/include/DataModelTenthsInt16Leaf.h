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

#ifndef DATA_MODEL_TENTHS_INT16_LEAF_H
#define DATA_MODEL_TENTHS_INT16_LEAF_H

#include "DataModelRetainedValueLeaf.h"

#include <stdint.h>

class DataModelNode;

class DataModelTenthsInt16Leaf : public DataModelRetainedValueLeaf {
   private:
        int16_t wholeNumber;
        uint8_t tenths;

    public:
        DataModelTenthsInt16Leaf(const char *name, DataModelNode *parent);
        void set(int16_t wholeNumber, uint8_t tenths);
        virtual void sendRetainedValue(DataModelSubscriber &subscriber) override;
};

#endif
