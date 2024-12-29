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

#ifndef DATA_MODEL_H
#define DATA_MODEL_H

#include "DataModelRoot.h"
#include "DataModelSubscriber.h"
#include "DataModelNode.h"
#include "DataModelUInt16Leaf.h"
#include "DataModelUInt32Leaf.h"

#include "TaskObject.h"

#include "StatCounter.h"

#include "StatsHolder.h"

#include <freertos/semphr.h>

#include <stddef.h>
#include <stdint.h>

const char dataModelLevelSeparator = '/';
const char dataModelMultiLevelWildcard = '#';
const char dataModelSingleLevelWildcard = '+';

const size_t maxTopicNameLength = 255;

class StatsManager;

class DataModel : public TaskObject, public StatsHolder {
    private:
        static constexpr size_t stackSize = 3 * 1024;
        static constexpr uint32_t lockTimeoutMs = 60 * 1000;

        DataModelRoot _rootNode;
        SemaphoreHandle_t subscriptionLock;
        uint16_t subscriptionCount;
        uint16_t retainedValues;
        StatCounter updates;

        DataModelNode _sysNode;
        DataModelNode _brokerNode;
        DataModelNode subscriptionsNode;
        DataModelUInt16Leaf subscriptionsCountLeaf;
        DataModelNode _messagesNode;
        DataModelNode retainedNode;
        DataModelUInt16Leaf retainedCountLeaf;
        DataModelNode dataModelNode;
        DataModelUInt32Leaf updatesLeaf;
        DataModelUInt32Leaf updateRateLeaf;

        virtual void task() override;
        virtual void exportStats(uint32_t msElapsed) override;
        void takeSubscriptionLock();
        void releaseSubscriptionLock();

    public:
        DataModel(StatsManager &statsManager);
        DataModelRoot &rootNode();
        bool subscribe(const char *topicFilter, DataModelSubscriber &subscriber, uint32_t cookie);
        void unsubscribe(const char *topicFilter, DataModelSubscriber &subscriber);
        void unsubscribeAll(DataModelSubscriber &subscriber);
        DataModelNode &sysNode();
        DataModelNode &brokerNode();
        DataModelNode &messagesNode();
        void dump();

        // The below method should probably be a friend method or something
        void leafUpdated();
        void leafSubscribedTo();
        void leafUnsubscribedFrom();
        void valueRetained();
        void retainedValueCleared();

        // Used for taking locks, but should go away later
        friend class DataModelRoot;
};

#endif // DATA_MODEL_H
