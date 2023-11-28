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

#ifndef DATA_MODEL_UINT32_LEAF_H
#define DATA_MODEL_UINT32_LEAF_H

#include "DataModelRetainedValueLeaf.h"

#include <stdint.h>

class DataModelUInt32Leaf : public DataModelRetainedValueLeaf {
   private:
        uint32_t value;

    public:
        DataModelUInt32Leaf(const char *name, DataModelElement *parent);
        DataModelUInt32Leaf & operator = (const uint32_t value);
        DataModelUInt32Leaf operator ++ (int);
        DataModelUInt32Leaf operator -- (int);
        operator uint32_t() const;
        virtual void sendRetainedValue(DataModelSubscriber &subscriber) override;
};

#endif
