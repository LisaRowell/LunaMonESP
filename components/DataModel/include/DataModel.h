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

#include "TaskObject.h"

#include <stddef.h>

const char dataModelLevelSeparator = '/';
const char dataModelMultiLevelWildcard = '#';
const char dataModelSingleLevelWildcard = '+';

const size_t maxTopicNameLength = 255;

class DataModel : public TaskObject {
    private:
        DataModelRoot _rootNode;

        virtual void task() override;

    public:
        DataModel();
        DataModelRoot &rootNode();
        bool subscribe(const char *topicFilter, DataModelSubscriber &subscriber, uint32_t cookie);
        void unsubscribe(const char *topicFilter, DataModelSubscriber &subscriber);
        void dump();

        // The below method should probably be a friend method or something
        void leafUpdated();
};

#endif // DATA_MODEL_H
