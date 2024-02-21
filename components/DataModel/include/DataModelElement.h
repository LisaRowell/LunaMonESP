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

#ifndef DATA_MODEL_ELEMENT_H
#define DATA_MODEL_ELEMENT_H

#include "etl/intrusive_links.h"

#include <stdint.h>
#include <stddef.h>

class DataModelNode;
class DataModelSubscriber;

const unsigned maxDataModelSubscribers = 5;

constexpr size_t siblingLinkId = 0;
typedef etl::forward_link<siblingLinkId> siblingLink;

class DataModelElement : public siblingLink {
    private:
        const char *name;
        DataModelNode *parent;

    protected:
        bool isMultiLevelWildcard(const char *topicFilter);
        bool topicFilterMatch(const char *topicFilter, unsigned &offsetToNextLevel,
                              bool &lastLevel);
        void buildTopicName(char *topicNameBuffer);

    public:
        DataModelElement(const char *name, DataModelNode *parent);
        const char *elementName() const;
        // Returns true if one or more subscriptions were made
        virtual bool subscribeIfMatching(const char *topicFilter, DataModelSubscriber &subscriber,
                                         uint32_t cookie) = 0;
        virtual void unsubscribeIfMatching(const char *topicFilter,
                                           DataModelSubscriber &subscriber) = 0;
        virtual bool subscribeAll(DataModelSubscriber &subscriber, uint32_t cookie) = 0;
        virtual void unsubscribeAll(DataModelSubscriber &subscriber) = 0;
        virtual void leafUpdated();
        virtual void dump();
};

#endif
