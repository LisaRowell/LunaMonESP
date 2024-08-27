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
#include "DataModelUInt32Leaf.h"

#include "etl/string.h"

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

#include <stddef.h>

class StatsManager;
class DataModel;

class Interface : public TaskObject, StatsHolder {
    private:
        static constexpr uint32_t lockTimeoutMs = 60 * 1000;

        const char *_name;
        enum InterfaceProtocol protocol;
        DataModelNode _interfaceNode;
        DataModelUInt32Leaf receivedBytesLeaf;
        DataModelUInt32Leaf receivedByteRateLeaf;

        virtual void exportStats(uint32_t msElapsed) override;

    protected:
        SemaphoreHandle_t writeLock;
        StatCounter receivedBytes;

        void takeWriteLock();
        void releaseWriteLock();

    public:
        Interface(const char *name, enum InterfaceProtocol protocol, StatsManager &statsManager,
                  DataModel &dataModel, size_t stackSize);
        DataModelNode &interfaceNode();
        const char *name() const;
        size_t send(const char *string);
        size_t send(const etl::istring &string);
        bool sendMessage(const char *string, bool blocking = true);
        bool sendMessage(const etl::istring &string, bool blocking = true);

        virtual size_t sendBytes(const void *bytes, size_t length) = 0;
        virtual bool sendMessageBytes(const void *bytes, size_t length, bool blocking = true) = 0;
};

#endif // INTERFACE_H
