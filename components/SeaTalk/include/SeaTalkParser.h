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

#ifndef SEATALK_PARSER_H
#define SEATALK_PARSER_H

#include "StatsHolder.h"

#include "InstrumentData.h"

#include "DataModelUInt32Leaf.h"
#include "DataModelStringLeaf.h"

#include "etl/set.h"
#include "etl/string.h"

#include <stddef.h>
#include <stdint.h>

class DataModelNode;
class SeaTalkLine;
class SeaTalkCommand;
class StatsManager;

class SeaTalkParser : StatsHolder {
    private:
        static constexpr size_t maxKnownDevices = 10;
        static constexpr size_t coordinateLength = 20;
        static constexpr size_t dateLength = 11;
        static constexpr size_t timeLength = 10;
        static constexpr size_t activeSatellitesLength = 72;
        static constexpr size_t knownDevicesLength = 10 * 3 + 1;

        InstrumentData &instrumentData;
        etl::set<uint8_t, maxKnownDevices> devicesSeen;
        uint32_t ignoredCommands;
        uint32_t unknownCommands;
        uint32_t commandLengthErrors;
        uint32_t commandFormatErrors;
        etl::string<knownDevicesLength> knownDevicesBuffer;
        DataModelUInt32Leaf ignoredCommandsLeaf;
        DataModelUInt32Leaf unknownCommandsLeaf;
        DataModelUInt32Leaf commandLengthErrorsLeaf;
        DataModelUInt32Leaf commandFormatErrorsLeaf;
        DataModelStringLeaf knownDevicesLeaf;

        void parseDepthBelowTransducer(const SeaTalkLine &seaTalkLine);
        void parseApparentWindAngle(const SeaTalkLine &seaTalkLine);
        void parseApparentWindSpeed(const SeaTalkLine &seaTalkLine);
        void parseSpeedThroughWaterV1(const SeaTalkLine &seaTalkLine);
        void parseWaterTemperatureV1(const SeaTalkLine &seaTalkLine);
        void parseTotalAndTripLog(const SeaTalkLine &seaTalkLine);
        void parseSpeedThroughWaterV2(const SeaTalkLine &seaTalkLine);
        void parseWaterTemperatureV2(const SeaTalkLine &seaTalkLine);
        void parseSetLampIntensity(const SeaTalkLine &seaTalkLine);
        void parseLatitudePosition(const SeaTalkLine &seaTalkLine);
        void parseLongitudePosition(const SeaTalkLine &seaTalkLine);
        void parseSpeedOverGround(const SeaTalkLine &seaTalkLine);
        void parseCourseOverGround(const SeaTalkLine &seaTalkLine);
        void parseTime(const SeaTalkLine &seaTalkKine);
        void parseDate(const SeaTalkLine &seaTalkKine);
        void parseSatelliteInfo(const SeaTalkLine &seaTalkLine);
        void parseRawLatitudeAndLongitude(const SeaTalkLine &seaTalkLine);
        void parseAutoPilotStatus(const SeaTalkLine &seaTalkLine);
        void parseAutoPilotHeadingCourseAndRudder(const SeaTalkLine &seaTalkLine);
        void parseDeviceIdentification(const SeaTalkLine &seaTalkLine);
        void parseMagneticVariation(const SeaTalkLine &seaTalkLine);
        void parseAutoPilotHeadingAndRudder(const SeaTalkLine &seaTalkLine);
        void parseGPSAndDGPSInfo(const SeaTalkLine &seaTalkLine);
        void parseGPSAndDGPSFixInfo(const SeaTalkLine &seaTalkLine);
        const char *parseSignalQuality(uint8_t signalQualityCode, bool signalQualityAvailable);
        void parseActiveSatellites(const SeaTalkLine &seaTalkLine);
        void ignoredCommand(const SeaTalkCommand &command, const SeaTalkLine &seaTalkLine);
        void unknownCommand(const SeaTalkCommand &command, const SeaTalkLine &seaTalkLine);
        bool checkLength(size_t expectedLength, const SeaTalkLine &seaTalkLine);
        bool checkAttribute(const SeaTalkLine &seaTalkLine, uint8_t expectedAttribute,
                            uint8_t mask = 0xff);
        virtual void exportStats(uint32_t msElapsed) override;
        const char *modeBitsToName(uint8_t modeBits) const;
        void coordinateToString(uint8_t degrees, uint16_t minutesX100, char suffix,
                                etl::istring &string) const;
        void rawCoordinateToString(uint8_t degrees, uint16_t minutesX1000, char suffix,
                                   etl::istring &string) const;

    public:
        SeaTalkParser(DataModelNode &inputNode, InstrumentData &instrumentData,
                      StatsManager &statsManager);
        void parseLine(const SeaTalkLine &seaTalkLine);
};

#endif // SEATALK_PARSER_H
