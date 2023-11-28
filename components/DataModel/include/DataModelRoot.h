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

#ifndef DATA_MODEL_ROOT_H
#define DATA_MODEL_ROOT_H

#include "DataModelNode.h"
#include "DataModelSubscriber.h"

#include <stdint.h>

class DataModel;

class DataModelRoot : public DataModelNode {
    private:
        DataModel *dataModel;

        bool checkTopicFilterValidity(const char *topicFilter);
        bool subscribeChildrenIfMatching(const char *topicFilter, DataModelSubscriber &subscriber,
                                         uint32_t cookie);

    public:
        DataModelRoot();
        void setDataModel(DataModel *dataModel);
        bool subscribe(const char *topicFilter, DataModelSubscriber &subscriber, uint32_t cookie);
        void unsubscribe(const char *topicFilter, DataModelSubscriber &subscriber);
        virtual bool subscribeAll(DataModelSubscriber &subscriber, uint32_t cookie) override;
        virtual void leafUpdated() override;
};

#endif
