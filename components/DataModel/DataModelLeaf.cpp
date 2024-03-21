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

#include "DataModelLeaf.h"
#include "DataModelNode.h"
#include "DataModel.h"
#include "DataModelSubscriber.h"

#include "Logger.h"

#include "etl/string.h"
#include "etl/to_string.h"

#include <stdint.h>
#include <stddef.h>

DataModelLeaf::DataModelLeaf(const char *name, DataModelNode *parent)
    : DataModelElement(name, parent) {

    unsigned subscriberPos;
    for (subscriberPos = 0; subscriberPos < maxDataModelSubscribers; subscriberPos++) {
        subscribers[subscriberPos] = NULL;
    }
}

bool DataModelLeaf::isSubscribed(DataModelSubscriber &subscriber) {
    unsigned subscriberPos;
    for (subscriberPos = 0; subscriberPos < maxDataModelSubscribers; subscriberPos++) {
        if (subscribers[subscriberPos] == &subscriber) {
            return true;
        }
    }

    return false;
}

bool DataModelLeaf::addSubscriber(DataModelSubscriber &subscriber, uint32_t cookie) {
    unsigned subscriberPos;
    for (subscriberPos = 0; subscriberPos < maxDataModelSubscribers; subscriberPos++) {
        if (subscribers[subscriberPos] == NULL) {
            subscribers[subscriberPos] = &subscriber;
            cookies[subscriberPos] = cookie;

            parent->leafSubscribedTo();

            return true;
        }
    }

    // This shouldn't happen if max sessions == max subscribers
    return false;
}

bool DataModelLeaf::updateSubscriber(DataModelSubscriber &subscriber, uint32_t cookie) {
    unsigned subscriberPos;
    for (subscriberPos = 0; subscriberPos < maxDataModelSubscribers; subscriberPos++) {
        if (subscribers[subscriberPos] == &subscriber) {
            cookies[subscriberPos] = cookie;
            return true;
        }
    }

    // This shouldn't happen if the call was used as expected. Error?
    return false;
}

void DataModelLeaf::unsubscribe(DataModelSubscriber &subscriber) {
    unsigned subscriberPos;
    for (subscriberPos = 0; subscriberPos < maxDataModelSubscribers; subscriberPos++) {
        if (subscribers[subscriberPos] == &subscriber) {
            subscribers[subscriberPos] = NULL;
            parent->leafUnsubscribedFrom();

            // We don't cache the full name of a topic, and instead store it in bits in the tree,
            // so we don't log the full name. If we switch to storing the name, this debug could be
            // made to be more specific
            logger() << logDebugDataModel << "Client '" << subscriber.name()
                     << "' unsubcribed from topic ending in '" << elementName() << "'" << eol;
            return;
        }
    }

    // MQTT protocol wise, it's an acceptable occurance for a broker to receive an unsubscribe for
    // a topic that it doesn't have an active subcription for. We didn't find the subscriber on the
    // topic, but just calmly return.
}

bool DataModelLeaf::subscribe(DataModelSubscriber &subscriber, uint32_t cookie) {
    if (isSubscribed(subscriber)) {
        return updateSubscriber(subscriber, cookie);
    } else {
        return addSubscriber(subscriber, cookie);
    }
}

bool DataModelLeaf::subscribeIfMatching(const char *topicFilter, DataModelSubscriber &subscriber,
                                        uint32_t cookie) {
    if (isMultiLevelWildcard(topicFilter)) {
        return subscribe(subscriber, cookie);
    }

    unsigned offsetToNextLevel;
    bool lastLevel;
    if (topicFilterMatch(topicFilter, offsetToNextLevel, lastLevel)) {
        if (lastLevel) {
            return subscribe(subscriber, cookie);
        } else {
            return false;
        }
    } else {
        return false;
    }
}

bool DataModelLeaf::subscribeAll(DataModelSubscriber &subscriber, uint32_t cookie) {
    logger() << logDebugDataModel << subscriber.name() << " subscribing to element ending in '"
             << elementName() << "' via subscription wildcard" << eol;

    return subscribe(subscriber, cookie);
}

DataModelLeaf & DataModelLeaf::operator << (const etl::istring &value) {
    // For now we take the subscription lock when publishing, but later this should go away when
    // updates become threaded.
    parent->takeSubscriptionLock();

    unsigned subscriberIndex;
    for (subscriberIndex = 0; subscriberIndex < maxDataModelSubscribers; subscriberIndex++) {
        DataModelSubscriber *subscriber = subscribers[subscriberIndex];
        if (subscriber != NULL) {
            // This could be made more efficent by building a topic name outside of this loop
            // instead of down in the publish routine...
            publishToSubscriber(*subscriber, value, false);
        }
    }

    parent->releaseSubscriptionLock();

    parent->leafUpdated();

    return *this;
}

DataModelLeaf & DataModelLeaf::operator << (uint32_t value) {
    etl::string<12> valueStr;
    etl::to_string(value, valueStr);
    *this << valueStr;

    return *this;
}

void DataModelLeaf::publishToSubscriber(DataModelSubscriber &subscriber, const etl::istring &value,
                                        bool retainedValue) {
    char topic[maxTopicNameLength];
    buildTopicName(topic);
    subscriber.publish(topic, value.c_str(), retainedValue);
}

void DataModelLeaf::unsubscribeIfMatching(const char *topicFilter,
                                          DataModelSubscriber &subscriber) {
    if (isMultiLevelWildcard(topicFilter)) {
        unsubscribe(subscriber);
        return;
    }

    unsigned offsetToNextLevel;
    bool lastLevel;
    if (topicFilterMatch(topicFilter, offsetToNextLevel, lastLevel)) {
        if (lastLevel) {
            unsubscribe(subscriber);
        }
    }
}

void DataModelLeaf::unsubscribeAll(DataModelSubscriber &subscriber) {
    unsubscribe(subscriber);
}
