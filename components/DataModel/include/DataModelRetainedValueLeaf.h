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

#ifndef DATA_MODEL_RETAINED_VALUE_LEAF_H
#define DATA_MODEL_RETAINED_VALUE_LEAF_H

#include "DataModelLeaf.h"

#include <stdint.h>

class DataModelNode;

class DataModelRetainedValueLeaf : public DataModelLeaf {
    private:
        bool hasBeenSet;
        static uint16_t retainedValues;

    protected:
        DataModelRetainedValueLeaf(const char *name, DataModelNode *parent);
        virtual bool subscribe(DataModelSubscriber &subscriber, uint32_t cookie) override;
        void updated();
        bool hasValue() const;
        virtual void sendRetainedValue(DataModelSubscriber &subscriber) = 0;

    public:
        void removeValue();
        static uint16_t retainedValueCount();
};

#endif
