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

#include "SeaTalkParser.h"
#include "SeaTalkCommand.h"
#include "SeaTalkLine.h"
#include "SeaTalkLampIntensity.h"

#include "StatCounter.h"
#include "StatsHolder.h"
#include "StatsManager.h"

#include "InstrumentData.h"
#include "WindData.h"
#include "WaterData.h"

#include "DataModelNode.h"
#include "DataModelUInt32Leaf.h"

#include "TenthsInt16.h"
#include "TenthsUInt16.h"
#include "HundredthsUInt16.h"

#include "Logger.h"

#include <stddef.h>
#include <stdint.h>

SeaTalkParser::SeaTalkParser(DataModelNode &interfaceNode, InstrumentData &instrumentData,
                             StatsManager &statsManager)
    : instrumentData(instrumentData),
      commandsReceivedCounter(),
      ignoredCommands(0),
      unknownCommands(0),
      commandLengthErrors(0),
      commandFormatErrors(0),
      seaTalkNode("seatalk", &interfaceNode),
      receivedNode("received", &seaTalkNode),
      commandsReceivedLeaf("commands", &receivedNode),
      commandseceiveRateLeaf("commandRate", &receivedNode),
      ignoredCommandsLeaf("ignoredCommands", &receivedNode),
      unknownCommandsLeaf("unknownCommands", &receivedNode),
      commandLengthErrorsLeaf("commandLengthErrors", &receivedNode),
      commandFormatErrorsLeaf("commandFormatErrors", &receivedNode) {
    statsManager.addStatsHolder(*this);
}

void SeaTalkParser::parseLine(const SeaTalkLine &seaTalkLine) {
    // This shouldn't actually be a thing since a command is denoted by the first byte, but it here
    // as a check against messed up $STALK messages.
    if (seaTalkLine.length() == 0) {
        logger() << logWarnSeaTalk << "Ignoring 0 length SeaTalk message" << eol;
        return;
    }

    SeaTalkCommand command = seaTalkLine.command();
    switch (command) {
        case SeaTalkCommand::DEPTHS_BELOW_TRANSDUCER:
            parseDepthBelowTransducer(seaTalkLine);
            break;
        case SeaTalkCommand::APPARENT_WIND_ANGLE:
            parseApparentWindAngle(seaTalkLine);
            break;
        case SeaTalkCommand::APPARENT_WIND_SPEED:
            parseApparentWindSpeed(seaTalkLine);
            break;
        case SeaTalkCommand::SPEED_THROUGH_WATER_V1:
            parseSpeedThroughWaterV1(seaTalkLine);
            break;
        case SeaTalkCommand::WATER_TEMPERATURE_V1:
            parseWaterTemperatureV1(seaTalkLine);
            break;
        case SeaTalkCommand::SPEED_THROUGH_WATER_V2:
            parseSpeedThroughWaterV2(seaTalkLine);
            break;
        case SeaTalkCommand::WATER_TEMPERATURE_V2:
            parseWaterTemperatureV2(seaTalkLine);
            break;
        case SeaTalkCommand::SET_LAMP_INTENSITY:
            parseSetLampIntensity(seaTalkLine);
            break;
        case SeaTalkCommand::DISPLAY_UNITS_MILEAGE_AND_SPEED:
            ignoredCommand(command, seaTalkLine);
            break;
        default:
            unknownCommand(command, seaTalkLine);
    }

    commandsReceivedCounter++;
}

void SeaTalkParser::parseDepthBelowTransducer(const SeaTalkLine &seaTalkLine) {
    if (!checkLength(5, seaTalkLine)) {
        return;
    }
    if (!checkAttribute(seaTalkLine, 0x02)) {
        return;
    }

    uint8_t byte2 = seaTalkLine[2];
    uint8_t byte3 = seaTalkLine[3];

    bool anchorAlarmActive = (byte2 & 0x80) != 0;
    // This is not completely correct as the depth might be fathoms when the Meters bit is set. The
    // only way to know would be to look at what the next message is and if it's a 0x65, then it's
    // fathoms. Maybe we could add some state here and postpone setting the depth node?
    bool depthIsMeters = ((byte2 & 0x40) != 0);
    bool transducerDefective = (byte2 & 0x04) != 0;
    bool deepAlarmActive = (byte2 & 0x02) != 0;
    bool shallowAlarmActive = (byte2 & 0x01) != 0;

    TenthsUInt16 depth;
    depth.setFromTenths((seaTalkLine[4] << 8) | byte3);

    WaterData &waterData = instrumentData.waterData();
    waterData.beginUpdates();
    if (depthIsMeters) {
        waterData.depthBelowTransducerMetersLeaf = depth;
    } else {
        waterData.depthBelowTransducerFeetLeaf = depth;
    }
    waterData.anchorDepthAlarmLeaf = anchorAlarmActive;
    waterData.shallowDepthAlarmLeaf = shallowAlarmActive;
    waterData.deepDepthAlarmLeaf = deepAlarmActive;
    waterData.depthTransducerDefectiveLeaf = transducerDefective;
    waterData.endUpdates();

    logger() << logDebugSeaTalk << "Depth " << depth;
    if (depthIsMeters) {
        logger() << " m";
    } else {
        logger() << "'";
    }
    if (anchorAlarmActive) {
        logger() << ", Anchor alarm active";
    }
    if (deepAlarmActive) {
        logger() << ", Deep alarm active";
    }
    if (shallowAlarmActive) {
        logger() << ", Shallow alarm active";
    }
    if (transducerDefective) {
        logger() << ", Defective";
    }
    logger() << eol;
}


void SeaTalkParser::parseApparentWindAngle(const SeaTalkLine &seaTalkLine) {
    if (!checkLength(4, seaTalkLine)) {
        return;
    }
    if (!checkAttribute(seaTalkLine, 0x01)) {
        return;
    }

    // While the SeaTalk reference engineering information states that the byte order in messages
    // is low byte, high byte, that doesn't seem to apply to this message...
    uint16_t angleX2 = (seaTalkLine[2] << 8) | seaTalkLine[3];
    TenthsUInt16 angle;
    angle.setFromTenths(angleX2 * 5);

    WindData &windData = instrumentData.windData();
    windData.beginUpdates();
    windData.apparentWindAngleLeaf = angle;
    windData.endUpdates();

    logger() << logDebugSeaTalk << "Apparent wind angle " << angle << eol;
}

void SeaTalkParser::parseApparentWindSpeed(const SeaTalkLine &seaTalkLine) {
    if (!checkLength(4, seaTalkLine)) {
        return;
    }
    if (!checkAttribute(seaTalkLine, 0x01)) {
        return;
    }

    uint8_t byte2 = seaTalkLine[2];
    uint8_t byte3 = seaTalkLine[3];
    if (byte3 > 9) {
        logger() << logWarnSeaTalk << "Bad Apparent Wind Speed message byte 3 value (" << Hex
                 << byte3 << "): " << seaTalkLine << eol;
        commandFormatErrors++;
        return;
    }

    bool speedIsMetersPerSec = (byte2 & 0x80) != 0;
    TenthsUInt16 speed(byte2 & 0x7f, byte3);

    WindData &windData = instrumentData.windData();
    windData.beginUpdates();
    if (speedIsMetersPerSec) {
        windData.apparentWindSpeedMPSLeaf = speed;
    } else {
        windData.apparentWindSpeedKnotsLeaf = speed;
    }
    windData.endUpdates();

    logger() << logDebugSeaTalk << "Apparent wind speed " << speed
             << (speedIsMetersPerSec ? " m/s" : " kn") << eol;
}

void SeaTalkParser::parseSpeedThroughWaterV1(const SeaTalkLine &seaTalkLine) {
    if (!checkLength(4, seaTalkLine)) {
        return;
    }
    if (!checkAttribute(seaTalkLine, 0x01)) {
        return;
    }

    TenthsUInt16 speed;
    speed.setFromTenths((seaTalkLine[3] << 8) | seaTalkLine[2]);

    WaterData &waterData = instrumentData.waterData();
    waterData.beginUpdates();
    waterData.waterSpeedKnotsLeaf = speed;
    waterData.endUpdates();

    logger() << logDebugSeaTalk << "Speed through water " << speed << " kn" << eol;
}

void SeaTalkParser::parseWaterTemperatureV1(const SeaTalkLine &seaTalkLine) {
    if (!checkLength(4, seaTalkLine)) {
        return;
    }
    if (!checkAttribute(seaTalkLine, 0x01, 0x0f)) {
        return;
    }

    bool sensorDefective = (seaTalkLine.attribute() & 0x40) != 0;
    uint8_t celsiusTemp = seaTalkLine[2];
    uint8_t fahrenheitTemp = seaTalkLine[3];

    // This needs more consideration. The sensor may also be outputing the more precise water
    // temperature command and we might want to use that value instead. Maybe detect if the better
    // message is being received and skip this one if it is?
    // Leaving as is will cause the MQTT subscribers to see a lot of updates where the temperature
    // needlessly changes back and forth between the rounded off version and the tenths degree
    // value.
    WaterData &waterData = instrumentData.waterData();
    waterData.beginUpdates();
    waterData.waterTemperatureCelsiusLeaf = celsiusTemp;
    waterData.waterTemperatureFahrenheitLeaf = fahrenheitTemp;
    waterData.waterTemperatureSensorDefectiveLeaf = sensorDefective;
    waterData.endUpdates();

    logger() << logDebugSeaTalk << "Water temperature " << celsiusTemp << "° C "
             << fahrenheitTemp<< "° F";
    if (sensorDefective) {
        logger() << " Defective";
    }
    logger() << eol;
}

void SeaTalkParser::parseSpeedThroughWaterV2(const SeaTalkLine &seaTalkLine) {
    if (!checkLength(7, seaTalkLine)) {
        return;
    }
    if (!checkAttribute(seaTalkLine, 0x04)) {
        return;
    }

    uint8_t flags = seaTalkLine[6];

    bool firstSensorValid = (flags & 0x40) == 0x40;
    HundredthsUInt16 firstSensorSpeed;
    firstSensorSpeed.setFromHundredths((seaTalkLine[3] << 8) | seaTalkLine[2]);

    bool hasAverageSpeed = (flags & 0x80) == 0x00;
    HundredthsUInt16 averageSpeed;
    bool hasSecondSensor;
    HundredthsUInt16 secondSensorSpeed;
    if (hasAverageSpeed) {
        hasSecondSensor = false;
        averageSpeed.setFromHundredths((seaTalkLine[5] << 8) | seaTalkLine[4]);
        secondSensorSpeed = 0;
    } else {
        hasSecondSensor = true;
        secondSensorSpeed.setFromHundredths((seaTalkLine[5] << 8) | seaTalkLine[4]);
        averageSpeed = 0;
    }

    bool averageSpeedCalculationStopped = (flags & 0x01) == 0x01;
    bool displaySpeedInMilesPerHour = (flags & 0x02) == 0x02;

    WaterData &waterData = instrumentData.waterData();
    waterData.beginUpdates();
    if (displaySpeedInMilesPerHour) {
        waterData.waterSpeedMPHLeaf = firstSensorSpeed;
        if (hasAverageSpeed) {
            waterData.waterAverageSpeedMPHLeaf = averageSpeed;
            waterData.waterAverageSpeedStoppedLeaf = averageSpeedCalculationStopped;
        } else {
            waterData.waterSpeedSecondSensorMPHLeaf = secondSensorSpeed;
        }
    } else {
        waterData.waterSpeedKnotsLeaf = firstSensorSpeed;
        if (hasAverageSpeed) {
            waterData.waterAverageSpeedKnotsLeaf = averageSpeed;
            waterData.waterAverageSpeedStoppedLeaf = averageSpeedCalculationStopped;
        } else {
            waterData.waterSpeedSecondSensorKnotsLeaf = secondSensorSpeed;
        }
    }
    waterData.endUpdates();

    logger() << logDebugSeaTalk << "Speed through water ";
    if (firstSensorValid) {
        logger() << firstSensorSpeed << (displaySpeedInMilesPerHour ? " mi/h" : " kn");
    } else {
        logger() << "Invalid";
    }
    if (hasAverageSpeed) {
        logger() << " average " << averageSpeed << (displaySpeedInMilesPerHour ? " mi/h" : " kn");
        if (averageSpeedCalculationStopped) {
            logger() << " stopped";
        }
    }
    if (hasSecondSensor) {
        logger() << " 2nd sensor " << secondSensorSpeed << (displaySpeedInMilesPerHour ? " mi/h" : " kn");
    }
    logger() << eol;
}

void SeaTalkParser::parseWaterTemperatureV2(const SeaTalkLine &seaTalkLine) {
    if (!checkLength(4, seaTalkLine)) {
        return;
    }
    if (!checkAttribute(seaTalkLine, 0x01)) {
        return;
    }

    uint16_t celsiusTempX10MPlus100 = (seaTalkLine[3] << 8) | seaTalkLine[2];
    int16_t celsiusTempX10 = celsiusTempX10MPlus100 - 100;
    TenthsInt16 celsiusTemp(celsiusTempX10 / 10, celsiusTempX10 % 10);

    // See the note on parseWaterTemperatureV1 for future improvements.
    WaterData &waterData = instrumentData.waterData();
    waterData.beginUpdates();
    waterData.waterTemperatureCelsiusLeaf = celsiusTemp;
    waterData.endUpdates();

    logger() << logDebugSeaTalk << "Water temperature " << celsiusTemp << "° C" << eol;
}

void SeaTalkParser::parseSetLampIntensity(const SeaTalkLine &seaTalkLine) {
    if (!checkLength(3, seaTalkLine)) {
        return;
    }
    if (!checkAttribute(seaTalkLine, 0x00)) {
        return;
    }

    uint8_t byte2 = seaTalkLine[2];
    if ((byte2 & 0xf0) != 0) {
        logger() << logWarnSeaTalk << "Bad Lamp Intensity message byte 2 value (" << Hex
                 << byte2 << "): " << seaTalkLine << eol;
        commandFormatErrors++;
        return;
    }

    SeaTalkLampIntensity lampIntensity(byte2);

    logger() << logDebugSeaTalk << "Lamp intensity " << lampIntensity << eol;
}

void SeaTalkParser::ignoredCommand(const SeaTalkCommand &command, const SeaTalkLine &seaTalkLine) {
    logger() << logDebugSeaTalk << "Ignoring " << command << " message: " << seaTalkLine << eol;

    ignoredCommands++;
}

void SeaTalkParser::unknownCommand(const SeaTalkCommand &command, const SeaTalkLine &seaTalkLine) {
    logger() << logWarnSeaTalk << "Ignoring " << seaTalkLine.length()
             << " byte unknown SeaTalk message (type " << Hex << (uint8_t)command << "): "
             << seaTalkLine << eol;

    unknownCommands++;
}

bool SeaTalkParser::checkLength(size_t expectedLength, const SeaTalkLine &seaTalkLine) {
    size_t length = seaTalkLine.length();

    if (length != expectedLength) {
        logger() << logWarnSeaTalk << seaTalkLine.command()
                 << " message with an unexpected length (" << length << "): " << seaTalkLine << eol;

        commandLengthErrors++;

        return false;
    }

    return true;
}

bool SeaTalkParser::checkAttribute(const SeaTalkLine &seaTalkLine, uint8_t expectedAttribute,
                                   uint8_t mask) {
    uint8_t attribute = seaTalkLine.attribute();

    if ((attribute & mask) != expectedAttribute) {
        logger() << logWarnSeaTalk << "Unsupported "<< seaTalkLine.command() << " attribute " << Hex
                 << attribute << ": " << seaTalkLine << eol;

        commandFormatErrors++;

        return false;
    }

    return true;
}

void SeaTalkParser::exportStats(uint32_t msElapsed) {
    commandsReceivedCounter.update(commandsReceivedLeaf, commandseceiveRateLeaf, msElapsed);
    ignoredCommandsLeaf = ignoredCommands;
    unknownCommandsLeaf = unknownCommands;
    commandLengthErrorsLeaf = commandLengthErrors;
    commandFormatErrorsLeaf = commandFormatErrors;
}
