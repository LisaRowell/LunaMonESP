/*
 * This file is part of LunaMon (https://github.com/LisaRowell/LunaMonESP)
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

#ifndef DATA_MODEL_NODE_H
#define DATA_MODEL_NODE_H

class DataModelSubscriber;

#include "DataModelElement.h"

#include "etl/intrusive_forward_list.h"

#include <stdint.h>

class DataModelNode : public DataModelElement {
    protected:
        etl::intrusive_forward_list<DataModelElement, siblingLink> children;

        bool subscribeChildrenIfMatching(const char *topicFilter, DataModelSubscriber &subscriber,
                                         uint32_t cookie);
        void unsubscribeChildrenIfMatching(const char *topicFilter,
                                           DataModelSubscriber &subscriber);

    public:
        DataModelNode(const char *name, DataModelElement *parent);
        void addChild(DataModelElement &element);
        virtual bool subscribeIfMatching(const char *topicFilter, DataModelSubscriber &subscriber,
                                         uint32_t cookie) override;
        virtual void unsubscribeIfMatching(const char *topicFilter,
                                           DataModelSubscriber &subscriber) override;
        virtual bool subscribeAll(DataModelSubscriber &subscriber, uint32_t cookie) override;
        virtual void unsubscribeAll(DataModelSubscriber &subscriber) override;
};

#endif
