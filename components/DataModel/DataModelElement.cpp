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

#include "DataModelElement.h"
#include "DataModelNode.h"
#include "DataModel.h"

#include "Logger.h"

#include <stdint.h>

DataModelElement::DataModelElement(const char *name, DataModelNode *parent)
    : name(name), parent(parent) {
    if (parent != nullptr) {
        parent->addChild(*this);
    }
}

bool DataModelElement::isMultiLevelWildcard(const char *topicFilter) {
    return (topicFilter[0] == dataModelMultiLevelWildcard) && (topicFilter[1] == 0);
}

bool DataModelElement::topicFilterMatch(const char *topicFilter, unsigned &offsetToNextLevel,
                                        bool &lastLevel) {
    if (topicFilter[0] == dataModelSingleLevelWildcard) {
        switch (topicFilter[1]) {
            case 0:
                offsetToNextLevel = 0;
                lastLevel = true;
                return true;

            case dataModelLevelSeparator:
                offsetToNextLevel = 2;
                lastLevel = topicFilter[offsetToNextLevel] == 0;
                return true;

            default:
                logger() << logErrorDataModel
                         << "Data Model TopicFilter was not properly checked and went through "
                            "invalid" << eol;
                return false;
        }
    } else {
        unsigned pos;
        for (pos = 0;
             (topicFilter[pos] != dataModelLevelSeparator) && (topicFilter[pos] != 0);
             pos++) {
            if (name[pos] != topicFilter[pos]) {
                return false;
            }
        }

        offsetToNextLevel = pos + 1;
        lastLevel = topicFilter[pos] == 0;

        return true;
    }
}

void DataModelElement::buildTopicName(char *topicNameBuffer) {
    // TODO: Make this more efficent!
    if (parent) {
        parent->buildTopicName(topicNameBuffer);
        if (topicNameBuffer[0] != 0) {
            strcat(topicNameBuffer, "/");
        }
        strcat(topicNameBuffer, name);
    } else {
        if (name) {
            strcpy(topicNameBuffer, name);
        } else {
            topicNameBuffer[0] = 0;
        }
    }
}

const char *DataModelElement::elementName() const {
    return name;
}

void DataModelElement::leafUpdated() {
    if (parent != nullptr) {
        parent->leafUpdated();
    }
}

void DataModelElement::dump() {
    char topic[maxTopicNameLength];
    buildTopicName(topic);
    taskLogger() << logDebugDataModel << topic << eol;
}
