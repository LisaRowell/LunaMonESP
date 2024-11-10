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

#ifndef SEA_TALK_NMEA_BRIDGE_H
#define SEA_TALK_NMEA_BRIDGE_H

#include "StatsHolder.h"
#include "StatCounter.h"

#include "DataModelNode.h"
#include "DataModelUInt32Leaf.h"
#include "DataModelStringLeaf.h"

#include "etl/string.h"

#include <stdint.h>

class NMEALineHandler;
class NMEAMsgType;
class StatsManager;
class DataModel;
class TenthsUInt16;

class SeaTalkNMEABridge : StatsHolder {
    private:
        static constexpr size_t maxLabelLength = 20;

        const char *talkerCode;
        NMEALineHandler &destination;
        StatCounter bridgedMessages;
        DataModelNode bridgeNode;
        etl::string<maxLabelLength> labelBuffer;
        DataModelStringLeaf labelLeaf;
        DataModelUInt32Leaf bridgedMessagesLeaf;
        DataModelUInt32Leaf bridgedMessageRateLeaf;

        void bridgeMessage(const NMEAMsgType &msgType, const etl::istring &message);
        virtual void exportStats(uint32_t msElapsed) override;
        const char *validityCode(bool valid);

    public:
        SeaTalkNMEABridge(const char *name, const char *label, const char *talkerCode,
                          NMEALineHandler &destination, StatsManager &statsManager,
                          DataModel &dataModel);
        void bridgeDBTMessage(const TenthsUInt16 &depthFeet);
        void bridgeHDMMessage(uint16_t heading);
        void bridgeMWVMessage(const TenthsUInt16 &windAngle, bool windAngleValid,
                              const TenthsUInt16 &windSpeedKN, bool windSpeedValid);
        void bridgeVHWMessage(uint16_t headingTrue, bool headingTrueValid, uint16_t headingMagnetic,
                              bool headingMagneticValid, const TenthsUInt16 &waterSpeedKN,
                              bool waterSpeedKNValid, const TenthsUInt16 &waterSpeedKMPH,
                              bool waterSpeedKMPHValid);
        void bridgeRSAMessage(int8_t stbdRudderPos, bool stbdRudderPosValid, int8_t portRudderPos,
                              bool portRudderPosValid);
};

#endif // SEA_TALK_NMEA_BRIDGE_H
