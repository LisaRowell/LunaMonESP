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

#include "DataModelNode.h"
#include "DataModelElement.h"
#include "DataModelSubscriber.h"

#include "Logger.h"

#include <stdint.h>

DataModelNode::DataModelNode(const char *name, DataModelElement *parent)
    : DataModelElement(name, parent) {
}

void DataModelNode::addChild(DataModelElement &element) {
    children.push_front(element);
}

bool DataModelNode::subscribeAll(DataModelSubscriber &subscriber, uint32_t cookie) {
    logger() << logDebugDataModel << subscriber.name()
             << " subscribing to children of node ending in '" << elementName()
             << "' via subscription wildcard" << eol;

    for (DataModelElement &child : children) {
        if (!child.subscribeAll(subscriber, cookie)) {
            return false;
        }
    }

    return true;
}

bool DataModelNode::subscribeIfMatching(const char *topicFilter, DataModelSubscriber &subscriber,
                                        uint32_t cookie) {
    if (isMultiLevelWildcard(topicFilter)) {
        return subscribeAll(subscriber, cookie);
    }

    unsigned offsetToNextLevel;
    bool lastLevel;
    if (topicFilterMatch(topicFilter, offsetToNextLevel, lastLevel)) {
        if (lastLevel) {
            // If we had non-leaf elements hold values, we'd do something interesting here, but
            // that's currently not a thing in the data model.
            return false;
        } else {
            const char *newTopicFilter = topicFilter + offsetToNextLevel;
            return subscribeChildrenIfMatching(newTopicFilter, subscriber, cookie);
        }
    } else {
        return false;
    }
}

bool DataModelNode::subscribeChildrenIfMatching(const char *topicFilter,
                                                DataModelSubscriber &subscriber,
                                                uint32_t cookie) {
    bool atLeastOneMatch = false;
    for (DataModelElement &child : children) {
        if (child.subscribeIfMatching(topicFilter, subscriber, cookie)) {
            atLeastOneMatch = true;
        }
    }

    return atLeastOneMatch;
}

void DataModelNode::unsubscribeIfMatching(const char *topicFilter,
                                          DataModelSubscriber &subscriber) {
    if (isMultiLevelWildcard(topicFilter)) {
        unsubscribeAll(subscriber);
        return;
    }

    unsigned offsetToNextLevel;
    bool lastLevel;
    if (topicFilterMatch(topicFilter, offsetToNextLevel, lastLevel)) {
        if (lastLevel) {
            // If we had non-leaf elements hold values, we'd do something interesting here, but
            // that's currently not a thing in the data model.
        } else {
            const char *newTopicFilter = topicFilter + offsetToNextLevel;
            unsubscribeChildrenIfMatching(newTopicFilter, subscriber);
        }
    }
}

void DataModelNode::unsubscribeChildrenIfMatching(const char *topicFilter,
                                                  DataModelSubscriber &subscriber) {
    for (DataModelElement &child : children) {
        child.unsubscribeIfMatching(topicFilter, subscriber);
    }
}

void DataModelNode::unsubscribeAll(DataModelSubscriber &subscriber) {
    //If we allowed intermediate nodes to hold values, we would need to do an unsubscribe here.

    for (DataModelElement &child : children) {
        child.unsubscribeAll(subscriber);
    }
}
