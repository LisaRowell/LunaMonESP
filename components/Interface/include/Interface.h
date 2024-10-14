/*
 * This file is part of LunaMon (https://github.com/LisaRowell/LunaMonESP)
 * Copyright (C) 2024 Lisa Rowell
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

#ifndef INTERFACE_H
#define INTERFACE_H

#include "TaskObject.h"
#include "InterfaceProtocol.h"

#include "StatsHolder.h"
#include "StatCounter.h"
#include "DataModelNode.h"
#include "DataModelStringLeaf.h"
#include "DataModelUInt32Leaf.h"

#include "etl/string.h"

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

#include <stddef.h>
#include <stdint.h>

class StatsManager;
class DataModel;

class Interface : public TaskObject, StatsHolder {
    private:
        static constexpr uint32_t lockTimeoutMs = 60 * 1000;
        static constexpr size_t maxLabelLength = 20;

        const char *_name;
        enum InterfaceProtocol protocol;
        DataModelNode _interfaceNode;
        etl::string<maxLabelLength> labelBuffer;
        StatCounter inputBytes;
        StatCounter outputBytes;
        DataModelStringLeaf labelLeaf;
        DataModelNode inputNode;
        DataModelUInt32Leaf inputBytesLeaf;
        DataModelUInt32Leaf inputByteRateLeaf;
        DataModelNode outputNode;
        DataModelUInt32Leaf outputBytesLeaf;
        DataModelUInt32Leaf outputByteRateLeaf;
        SemaphoreHandle_t writeLock;

        virtual void exportStats(uint32_t msElapsed) override;

    protected:
        void takeWriteLock();
        void releaseWriteLock();

    public:
        Interface(const char *name, const char *label, enum InterfaceProtocol protocol,
                  StatsManager &statsManager, DataModel &dataModel, size_t stackSize);
        DataModelNode &interfaceNode();
        const char *name() const;
        void countReceived(size_t bytes);
        void countSent(size_t bytes);
        size_t send(const char *string);
        size_t send(const etl::istring &string);
        virtual size_t sendBytes(const void *bytes, size_t length) = 0;
};

#endif // INTERFACE_H
