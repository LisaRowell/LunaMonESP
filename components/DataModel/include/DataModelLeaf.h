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

#ifndef DATA_MODEL_LEAF_H
#define DATA_MODEL_LEAF_H

class DataModelSubscriber;

#include "DataModelElement.h"

#include "etl/string.h"

#include <stdint.h>

class DataModelLeaf : public DataModelElement {
    private:
        DataModelSubscriber *subscribers[maxDataModelSubscribers];
        uint32_t cookies[maxDataModelSubscribers];

        bool addSubscriber(DataModelSubscriber &subscriber, uint32_t cookie);

    protected:
        bool isSubscribed(DataModelSubscriber &subscriber);
        bool updateSubscriber(DataModelSubscriber &subscriber, uint32_t cookie);
        virtual bool subscribe(DataModelSubscriber &subscriber, uint32_t cookie);
        void unsubscribe(DataModelSubscriber &subscriber);
        void publishToSubscriber(DataModelSubscriber &subscriber, const etl::istring &value,
                                 bool retainedValue);

    public:
        DataModelLeaf(const char *name, DataModelElement *parent);
        virtual bool subscribeIfMatching(const char *topicFilter, DataModelSubscriber &subscriber,
                                         uint32_t cookie) override;
        virtual bool subscribeAll(DataModelSubscriber &subscriber, uint32_t cookie) override;
        virtual void unsubscribeIfMatching(const char *topicFilter,
                                           DataModelSubscriber &subscriber) override;
        virtual void unsubscribeAll(DataModelSubscriber &subscriber) override;
        DataModelLeaf & operator << (const etl::istring &value);
        DataModelLeaf & operator << (uint32_t value);
};

#endif
