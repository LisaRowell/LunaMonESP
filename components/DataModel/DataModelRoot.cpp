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

#include "DataModelRoot.h"
#include "DataModel.h"
#include "DataModelNode.h"
#include "DataModelElement.h"

#include "Logger.h"

#include <stdint.h>

DataModelRoot::DataModelRoot()
    : DataModelNode(NULL, NULL), dataModel(nullptr) {
}

void DataModelRoot::setDataModel(DataModel *dataModel) {
    this->dataModel = dataModel;
}

bool DataModelRoot::subscribe(const char *topicFilter, DataModelSubscriber &subscriber,
                              uint32_t cookie) {
    if (!checkTopicFilterValidity(topicFilter )) {
        logger() << logWarnDataModel << "Illegal Topic Filter '" << topicFilter << "'" << eol;
        return false;
    }

    if (isMultiLevelWildcard(topicFilter)) {
        logger() << logDebugDataModel << subscriber.name() << " subscribing to all (#)" << eol;
        return subscribeAll(subscriber, cookie);
    }

    return subscribeChildrenIfMatching(topicFilter, subscriber, cookie);
}

bool DataModelRoot::subscribeAll(DataModelSubscriber &subscriber, uint32_t cookie) {
    logger() << logDebugDataModel << subscriber.name()
             << " subscribing to children of root node via multilevel wildcard" << eol;

    for (DataModelElement &child : children) {
        if (child.elementName()[0] != '$') {
            if (!child.subscribeAll(subscriber, cookie)) {
                return false;
            }
        }
    }

    return true;
}

bool DataModelRoot::subscribeChildrenIfMatching(const char *topicFilter,
                                                DataModelSubscriber &subscriber, uint32_t cookie) {
    const bool isSingleLevelWildcard = topicFilter[0] == dataModelSingleLevelWildcard;

    bool atLeastOneMatch = false;
    for (DataModelElement &child : children) {
        // Per the MQTT specification, single level wildcards must not match with topics beginning
        // with a $
        if (!(isSingleLevelWildcard && child.elementName()[0] == '$')) {
            if (child.subscribeIfMatching(topicFilter, subscriber, cookie)) {
                atLeastOneMatch = true;
            }
        }
    }

    return atLeastOneMatch;
}

void DataModelRoot::unsubscribe(const char *topicFilter, DataModelSubscriber &subscriber) {
    if (!checkTopicFilterValidity(topicFilter )) {
        logger() << logWarnDataModel << "Illegal Topic Filter '" << topicFilter
                 << "' in unsubscribe from Client '" << subscriber.name() << eol;
        return;
    }

    if (isMultiLevelWildcard(topicFilter)) {
        unsubscribeAll(subscriber);
    } else {
        unsubscribeChildrenIfMatching(topicFilter, subscriber);
    }
}

bool DataModelRoot::checkTopicFilterValidity(const char *topicFilter) {
    if (topicFilter[0] == 0) {
        return false;
    }

    unsigned pos;
    char previousChar;
    char currentChar;
    char nextChar;
    for (pos = 0, previousChar = 0, currentChar = topicFilter[0], nextChar = topicFilter[1];
         topicFilter[pos];
         pos++, previousChar = currentChar, currentChar = nextChar,
         nextChar = topicFilter[pos + 1]) {
        switch (currentChar) {
            case dataModelMultiLevelWildcard:
                return (previousChar == 0 || previousChar == dataModelLevelSeparator) &&
                       (nextChar == 0);

            case dataModelSingleLevelWildcard:
                if (previousChar != 0 && previousChar != dataModelLevelSeparator) {
                    return false;
                }
                if (nextChar != 0 && nextChar != dataModelLevelSeparator) {
                    return false;
                }
                break;

            case dataModelLevelSeparator:
                if (previousChar == dataModelLevelSeparator) {
                    return false;
                }
                break;

            default:
                break;
        }
    }

    return true;
}

void DataModelRoot::leafUpdated() {
    if (dataModel != nullptr) {
        dataModel->leafUpdated();
    }
}