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

/*
 * The SeaTalk decoding contained in this file is mostly on the work of
 * Thomas Knauf and his many contributers. Much gratitude is owned to their
 * work in compiling what's know about the SeaTalk protocol. Documentation
 * can be found at:
 * http://www.thomasknauf.de/seatalk.htm
 *
 * I've tried to know in the below code where observations based on my boat's
 * instrumentation have differed from what has been documented. If I get to
 * the point of higher confidence in my difference I'll push them upstream.
*/
#include "SeaTalkParser.h"
#include "SeaTalkCommand.h"
#include "SeaTalkLine.h"
#include "SeaTalkLampIntensity.h"

#include "StatsHolder.h"
#include "StatsManager.h"

#include "InstrumentData.h"
#include "AutoPilotData.h"
#include "GPSData.h"
#include "WindData.h"
#include "WaterData.h"

#include "DataModelNode.h"
#include "DataModelUInt32Leaf.h"

#include "TenthsInt16.h"
#include "TenthsUInt16.h"
#include "HundredthsUInt16.h"
#include "HundredthsUInt8.h"

#include "Logger.h"

#include "etl/string.h"
#include "etl/string_stream.h"
#include "etl/set.h"
#include "etl/to_string.h"

#include <stddef.h>
#include <stdint.h>

SeaTalkParser::SeaTalkParser(DataModelNode &inputNode, InstrumentData &instrumentData,
                             StatsManager &statsManager)
    : instrumentData(instrumentData),
      ignoredCommands(0),
      unknownCommands(0),
      commandLengthErrors(0),
      commandFormatErrors(0),
      ignoredCommandsLeaf("ignoredCommands", &inputNode),
      unknownCommandsLeaf("unknownCommands", &inputNode),
      commandLengthErrorsLeaf("lengthErrors", &inputNode),
      commandFormatErrorsLeaf("formatErrors", &inputNode),
      knownDevicesLeaf("knownDevices", &inputNode, knownDevicesBuffer) {
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
        case SeaTalkCommand::LATITUDE_POSITION:
            parseLatitudePosition(seaTalkLine);
            break;
        case SeaTalkCommand::LONGITUDE_POSITION:
            parseLongitudePosition(seaTalkLine);
            break;
        case SeaTalkCommand::SPEED_OVER_GROUND:
            parseSpeedOverGround(seaTalkLine);
            break;
        case SeaTalkCommand::COURSE_OVER_GROUND:
            parseCourseOverGround(seaTalkLine);
            break;
        case SeaTalkCommand::HOURS_MINUTES_SECONDS:
            parseTime(seaTalkLine);
            break;
        case SeaTalkCommand::YEAR_MONTH_DAY:
            parseDate(seaTalkLine);
            break;
        case SeaTalkCommand::SATELLITE_INFO:
            parseSatelliteInfo(seaTalkLine);
            break;
        case SeaTalkCommand::RAW_LATITUDE_AND_LONGITUDE:
            parseRawLatitudeAndLongitude(seaTalkLine);
            break;
        case SeaTalkCommand::AUTO_PILOT_STATUS:
            parseAutoPilotStatus(seaTalkLine);
            break;
        case SeaTalkCommand::AUTO_PILOT_HEADING_COURSE_RUDDER:
            parseAutoPilotHeadingCourseAndRudder(seaTalkLine);
            break;
        case SeaTalkCommand::DEVICE_IDENTIFICATION:
            parseDeviceIdentification(seaTalkLine);
            break;
        case SeaTalkCommand::MAGNETIC_VARIATION:
            parseMagneticVariation(seaTalkLine);
            break;
        case SeaTalkCommand::AUTO_PILOT_HEADING_AND_RUDDER:
            parseAutoPilotHeadingAndRudder(seaTalkLine);
            break;
        case SeaTalkCommand::GPS_AND_DGPS_INFO:
            parseGPSAndDGPSInfo(seaTalkLine);
            break;
        case SeaTalkCommand::DISPLAY_UNITS_MILEAGE_AND_SPEED:
        case SeaTalkCommand::COUNT_DOWN_TIMER:
        case SeaTalkCommand::UNKNOWN_COMMAND_60:
        case SeaTalkCommand::E80_START_UP:
        case SeaTalkCommand::UNKNOWN_GPS_COMMAND_A7:
            ignoredCommand(command, seaTalkLine);
            break;
        default:
            unknownCommand(command, seaTalkLine);
    }
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

void SeaTalkParser::parseLatitudePosition(const SeaTalkLine &seaTalkLine) {
    if (!checkLength(5, seaTalkLine)) {
        return;
    }
    if (!checkAttribute(seaTalkLine, 0x02, 0x0f)) {
        return;
    }

    uint8_t degrees = seaTalkLine[2];
    uint16_t minutesX100 = (uint16_t)(seaTalkLine[4] & 0x7f) << 8 | seaTalkLine[3];
    char suffix = (seaTalkLine[4] & 0x80) == 0 ? 'N' : 'S';

    etl::string<coordinateLength> latitudeStr;
    coordinateToString(degrees, minutesX100, suffix, latitudeStr);

    GPSData &gpsData = instrumentData.gpsData();
    gpsData.beginUpdates();
    gpsData.latitudeLeaf = latitudeStr;
    gpsData.endUpdates();

    logger() << logDebugSeaTalk << "Latitude " << latitudeStr << eol;
}

void SeaTalkParser::parseLongitudePosition(const SeaTalkLine &seaTalkLine) {
    if (!checkLength(5, seaTalkLine)) {
        return;
    }
    if (!checkAttribute(seaTalkLine, 0x02, 0x0f)) {
        return;
    }

    uint8_t degrees = seaTalkLine[2];
    uint16_t minutesX100 = (uint16_t)(seaTalkLine[4] & 0x7f) << 8 | seaTalkLine[3];
    char suffix = (seaTalkLine[4] & 0x80) == 0 ? 'W' : 'E';

    etl::string<coordinateLength> longitudeStr;
    coordinateToString(degrees, minutesX100, suffix, longitudeStr);

    GPSData &gpsData = instrumentData.gpsData();
    gpsData.beginUpdates();
    gpsData.longitudeLeaf = longitudeStr;
    gpsData.endUpdates();

    logger() << logDebugSeaTalk << "Longitude " << longitudeStr << eol;
}

void SeaTalkParser::parseSpeedOverGround(const SeaTalkLine &seaTalkLine) {
    if (!checkLength(4, seaTalkLine)) {
        return;
    }
    if (!checkAttribute(seaTalkLine, 0x01)) {
        return;
    }

    uint16_t speedOverGroundX10 = (uint16_t)(seaTalkLine[3] << 8) | seaTalkLine[2];
    TenthsUInt16 speedOverGround;
    speedOverGround.setFromTenths(speedOverGroundX10);

    GPSData &gpsData = instrumentData.gpsData();
    gpsData.beginUpdates();
    gpsData.speedOverGroundLeaf = speedOverGround;
    gpsData.endUpdates();

    logger() << logDebugSeaTalk << "Speed Over Ground " << speedOverGround << " kn" << eol;
}

void SeaTalkParser::parseCourseOverGround(const SeaTalkLine &seaTalkLine) {
    if (!checkLength(3, seaTalkLine)) {
        return;
    }
    if (!checkAttribute(seaTalkLine, 0x00, 0x0f)) {
        return;
    }

    uint8_t attributeBits = seaTalkLine[1] & 0xf0;
    uint16_t courseOverGroundX2
        = ((uint16_t)(attributeBits & 0x30) >> 4) * 90 * 2 + ((seaTalkLine[2] & 0x3f) << 1) * 2 +
          ((attributeBits & 0xc0) >> 6);
    TenthsUInt16 courseOverGround;
    courseOverGround.setFromTenths(courseOverGroundX2 * 5);

    GPSData &gpsData = instrumentData.gpsData();
    gpsData.beginUpdates();
    gpsData.trackMadeGoodMagneticLeaf = courseOverGround;
    gpsData.endUpdates();

    logger() << logDebugSeaTalk << "Course Over Ground " << courseOverGround << "\xC2\xB0" << eol;
}

void SeaTalkParser::parseTime(const SeaTalkLine &seaTalkLine) {
    if (!checkLength(4, seaTalkLine)) {
        return;
    }
    if (!checkAttribute(seaTalkLine, 0x01, 0x0f)) {
        return;
    }

    uint8_t seconds = ((seaTalkLine[2] & 0x03) << 4) | ((seaTalkLine[1] & 0xf0) >> 4);
    uint8_t minutes = (seaTalkLine[2] & 0xfc) >> 2;
    uint8_t hours = seaTalkLine[3];

    etl::string<timeLength> timeStr;
    etl::string_stream timeStrStream(timeStr);
    timeStrStream << etl::setfill('0') << etl::setw(2) << hours << etl::setw(1) << ":"
                  << etl::setw(2) << minutes << etl::setw(1) << ":" << etl::setw(2) << seconds;

    GPSData &gpsData = instrumentData.gpsData();
    gpsData.beginUpdates();
    gpsData.timeLeaf = timeStr;
    gpsData.endUpdates();

    logger() << logDebugSeaTalk << "Time: " << timeStr << eol;
}

void SeaTalkParser::parseDate(const SeaTalkLine &seaTalkLine) {
    if (!checkLength(4, seaTalkLine)) {
        return;
    }
    if (!checkAttribute(seaTalkLine, 0x01, 0x0f)) {
        return;
    }

    uint8_t month = ((seaTalkLine[1] & 0xf0) >> 4);
    uint8_t day = seaTalkLine[2];
    uint16_t year = seaTalkLine[3] + 2000;

    etl::string<dateLength> dateStr;
    etl::string_stream dateStrStream(dateStr);
    dateStrStream << etl::setfill('0') << etl::setw(2) << month << etl::setw(1) << "/"
                  << etl::setw(2) << day << etl::setw(1) << "/" << etl::setw(4) << year;

    GPSData &gpsData = instrumentData.gpsData();
    gpsData.beginUpdates();
    gpsData.dateLeaf = dateStr;
    gpsData.endUpdates();

    logger() << logDebugSeaTalk << "Date: " << dateStr << " " << seaTalkLine << eol;
}

void SeaTalkParser::parseSatelliteInfo(const SeaTalkLine &seaTalkLine) {
    if (!checkLength(3, seaTalkLine)) {
        return;
    }
    if (!checkAttribute(seaTalkLine, 0x00, 0x0f)) {
        return;
    }

    uint8_t numberSatellites = (seaTalkLine[1] & 0xf0) >> 4;
    uint8_t horizontalDilutionOfPrecision = seaTalkLine[2];

    GPSData &gpsData = instrumentData.gpsData();
    gpsData.beginUpdates();
    gpsData.numberSatellitesLeaf = numberSatellites;
    gpsData.horizontalDilutionOfPrecisionLeaf = horizontalDilutionOfPrecision;
    gpsData.endUpdates();

    logger() << logDebugSeaTalk << "Number Satellites " << numberSatellites
             << " HorizontalDilutionOfPrecision " << horizontalDilutionOfPrecision << eol;
}

// We decode the raw latitude and longitude commands for debugging purposes, but do not export
// the values as they could be used in a misleading way.
void SeaTalkParser::parseRawLatitudeAndLongitude(const SeaTalkLine &seaTalkLine) {
    if (!checkLength(8, seaTalkLine)) {
        return;
    }
    if (!checkAttribute(seaTalkLine, 0x05, 0x0f)) {
        return;
    }

    uint8_t latitudeDegrees = seaTalkLine[2];
    uint16_t latitudeMinutesX1000 = (uint16_t)seaTalkLine[3] << 8 | seaTalkLine[4];
    uint8_t longitudeDegrees = seaTalkLine[5];
    uint16_t longitudeMinutesX1000 = (uint16_t)seaTalkLine[6] << 8 | seaTalkLine[7];
    bool isSouth = (seaTalkLine[1] & 0x10) != 0;
    bool isEast = (seaTalkLine[1] & 0x20) != 0;

    etl::string<coordinateLength> latitudeStr;
    rawCoordinateToString(latitudeDegrees, latitudeMinutesX1000, isSouth ? 'S' : 'N', latitudeStr);
    etl::string<coordinateLength> longitudeStr;
    rawCoordinateToString(longitudeDegrees, longitudeMinutesX1000, isEast ? 'E' : 'W',
                          longitudeStr);

    logger() << logDebugSeaTalk << "Raw Position " << latitudeStr << " " << longitudeStr << eol;
}

void SeaTalkParser::parseAutoPilotStatus(const SeaTalkLine &seaTalkLine) {
    if (!checkLength(10, seaTalkLine)) {
        return;
    }
    if (!checkAttribute(seaTalkLine, 0x07)) {
        return;
    }

    uint8_t status = seaTalkLine[2];
    etl::string<20> statusStr;
    switch (status) {
        case 0x00:
            statusStr = "Okay";
            break;
        case 0x01:
            statusStr = "Auto Release Error";
            break;
        case 0x08:
            statusStr = "Drive Stopped";
            break;
        default:
            etl::string_stream statusStream(statusStr);
            statusStream << "Unknown Error (0x" << etl::hex << etl::setw(2) << etl::setfill('0')
                         << status << ")";
    }

    AutoPilotData &autoPilotData = instrumentData.autoPilotData();
    autoPilotData.beginUpdates();
    autoPilotData.statusLeaf = statusStr;
    autoPilotData.endUpdates();

    logger() << logDebugSeaTalk << "Auto Pilot Status: " << statusStr << eol;
}

void SeaTalkParser::parseAutoPilotHeadingCourseAndRudder(const SeaTalkLine &seaTalkLine) {
    if (!checkLength(9, seaTalkLine)) {
        return;
    }
    if (!checkAttribute(seaTalkLine, 0x06, 0x0f)) {
        return;
    }

    uint8_t attributeByte = seaTalkLine[1];
    uint8_t upperAttributeBits = attributeByte & 0xc0;
    uint8_t upperAttributeBitsSet =
        ((upperAttributeBits & 0xc0) ? ((upperAttributeBits & 0xc0) == 0xc0 ? 2 : 1): 0);
    uint16_t heading = ((attributeByte & 0x30) >> 4) * 90 + (seaTalkLine[2] & 0x3f) * 2 +
                           upperAttributeBitsSet;
    uint16_t courseX2 = (uint16_t)((seaTalkLine[2] & 0xc0) >> 6) * 90 * 2 + seaTalkLine[3];
    TenthsUInt16 course;
    course.setFromTenths(courseX2 * 5);
    const char *mode = modeBitsToName(seaTalkLine[4] & 0x0f);
    uint8_t alarms = seaTalkLine[5] & 0x0f;
    bool offCourseAlarm = (alarms & 0x4) != 0;
    bool windShiftAlarm = (alarms & 0x8) != 0;
    int8_t rudderPosition = (int8_t)seaTalkLine[6];

    AutoPilotData &autoPilotData = instrumentData.autoPilotData();
    autoPilotData.beginUpdates();
    autoPilotData.headingSensorLeaf = heading;
    autoPilotData.courseLeaf = course;
    autoPilotData.modeLeaf = mode;
    autoPilotData.offCourseAlarmLeaf = offCourseAlarm;
    autoPilotData.windShiftAlarmLeaf = windShiftAlarm;
    autoPilotData.rudderCenterLeaf = rudderPosition;
    autoPilotData.endUpdates();

    logger() << logDebugSeaTalk << "Heading " << heading << " Course " << course << " Mode " << mode
             << " Rudder " << rudderPosition;
    if (offCourseAlarm) {
        logger() << " Off course";
    }
    if (windShiftAlarm) {
        logger() << " Wind shift";
    }
    logger() << eol;
}

void SeaTalkParser::parseDeviceIdentification(const SeaTalkLine &seaTalkLine) {
    if (!checkLength(3, seaTalkLine)) {
        return;
    }
    if (!checkAttribute(seaTalkLine, 0x00)) {
        return;
    }

    uint8_t deviceId = seaTalkLine[2];

    logger() << logDebugSeaTalk << "Device Identification: " << Hex << deviceId << eol;

    if (!devicesSeen.contains(deviceId) && !devicesSeen.full()) {
        devicesSeen.insert(deviceId);

        etl::string<maxKnownDevices * 3 + 1> knownDevicesString;
        etl::string_stream knownDevicesStream(knownDevicesString);
        bool firstDevice = true;
        for (const uint8_t &knownDevice : devicesSeen) {
            if (!firstDevice) {
                knownDevicesStream << ",";
            } else {
                firstDevice = false;
            }
            knownDevicesStream << etl::hex << etl::setw(2) << etl::setfill('0') << knownDevice
                               << etl::setw(1);
        }

        knownDevicesLeaf = knownDevicesString;
        logger() << logDebugSeaTalk << "Known Devices: " << knownDevicesString << eol;
    }
}

void SeaTalkParser::parseMagneticVariation(const SeaTalkLine &seaTalkLine) {
    if (!checkLength(3, seaTalkLine)) {
        return;
    }
    if (!checkAttribute(seaTalkLine, 0x00)) {
        return;
    }

    // We should not rely on the sign of this being interpretted correctly
    // ToDo: Test to make sure it makes sense.
    int8_t magneticVariation = (int8_t)seaTalkLine[2];

    GPSData &gpsData = instrumentData.gpsData();
    gpsData.beginUpdates();
    gpsData.magneticVariationLeaf = magneticVariation;
    gpsData.endUpdates();

    logger() << logDebugSeaTalk << "Magnetic Variation " << magneticVariation << eol;

}

void SeaTalkParser::parseAutoPilotHeadingAndRudder(const SeaTalkLine &seaTalkLine) {
    if (!checkLength(4, seaTalkLine)) {
        return;
    }
    if (!checkAttribute(seaTalkLine, 0x01, 0x0f)) {
        return;
    }

    uint8_t attributeByte = seaTalkLine[1];
    uint8_t upperAttributeBits = attributeByte & 0xc0;
    uint8_t upperAttributeBitsSet =
        ((upperAttributeBits & 0xc0) ? ((upperAttributeBits & 0xc0) == 0xc0 ? 2 : 1): 0);
    uint16_t heading = ((attributeByte & 0x30) >> 4) * 90 + (seaTalkLine[2] & 0x3f) * 2 +
                           upperAttributeBitsSet;

    int8_t rudderPosition = (int8_t)seaTalkLine[3];

    AutoPilotData &autoPilotData = instrumentData.autoPilotData();
    autoPilotData.beginUpdates();
    autoPilotData.headingSensorLeaf = heading;
    autoPilotData.rudderCenterLeaf = rudderPosition;
    autoPilotData.endUpdates();

    logger() << logDebugSeaTalk << "Heading " << heading << " Rudder " << rudderPosition << eol;
}

void SeaTalkParser::parseGPSAndDGPSInfo(const SeaTalkLine &seaTalkLine) {
    // Unlike other commands, this datagram can contain a varity of different pieces of information,
    // only some of which have been reverse engineered. The attributes byte is used to determine
    // what the message contains.
    switch (seaTalkLine.attribute()) {
        case 0x57:
            parseGPSAndDGPSFixInfo(seaTalkLine);
            break;
        case 0x74:
            parseActiveSatellites(seaTalkLine);
            break;
        default:
            logger() << logDebugSeaTalk << "Ignoring GPS and DPGS subcommand "
                     << Hex << seaTalkLine.attribute() << ": " << seaTalkLine << eol;
    }
}

void SeaTalkParser::parseGPSAndDGPSFixInfo(const SeaTalkLine &seaTalkLine) {
    uint8_t signalQualityCode = seaTalkLine[2] & 0x0f;
    bool signalQualityAvailable = (seaTalkLine[2] & 0x10) == 0x10;
    const char *signalQualityDescription = parseSignalQuality(signalQualityCode,
                                                              signalQualityAvailable);
    uint8_t hdop = seaTalkLine[3] & 0x7c;
    bool hdopAvailable = (seaTalkLine[3] & 0x80) == 0x80;
    int8_t antennaHeight = (int8_t)seaTalkLine[5];
    uint8_t numberSatellites = ((seaTalkLine[2] & 0xe0) >> 4) | (seaTalkLine[3] & 0x01);
    bool numberSatellitesAvailable = (seaTalkLine[3] & 0x02) == 0x02;
    int16_t geoSeperation = seaTalkLine[6] * 16;
    uint16_t differentialAge = (seaTalkLine[7] & 0xe0) >> 2 | (seaTalkLine[8] & 0x0f);
    bool differentialAgeAvailable = (seaTalkLine[8] & 0x10) == 0x10;
    uint16_t differentialStationID = (((uint16_t)seaTalkLine[8] & 0xc0) << 2) | seaTalkLine[9];
    bool differentialStationIDAvailable = (seaTalkLine[8] & 0x20) == 0x20;

    GPSData &gpsData = instrumentData.gpsData();
    gpsData.beginUpdates();
    gpsData.gpsQualityLeaf = signalQualityDescription;
    if (hdopAvailable) {
        gpsData.hdopLeaf = hdop;
    } else {
        gpsData.hdopLeaf.removeValue();
    }
    gpsData.altitudeLeaf = antennaHeight;
    if (numberSatellitesAvailable) {
        gpsData.numberSatellitesLeaf = numberSatellites;
    } else {
        gpsData.numberSatellitesLeaf.removeValue();
    }
    gpsData.geoidalSeparationLeaf = geoSeperation;
    if (differentialAgeAvailable) {
        gpsData.dataAgeLeaf = differentialAgeAvailable;
    } else {
        gpsData.dataAgeLeaf.removeValue();
    }
    if (differentialStationIDAvailable) {
        gpsData.differentialReferenceStationLeaf = differentialStationID;
    } else {
        gpsData.differentialReferenceStationLeaf.removeValue();
    }
    gpsData.endUpdates();

    logger() << logDebugSeaTalk << "Signal quality " << signalQualityDescription << " HDOP ";
    if (hdopAvailable) {
        logger() << hdop;
    } else {
        logger() << "unavailable";
    }
    logger() << " Antenna height " << antennaHeight << " # Satellites ";
    if (numberSatellitesAvailable) {
        logger() << numberSatellites;
    } else {
        logger() << "unavailable";
    }
    logger() << " Geo seperation " << geoSeperation << " Differential age ";
    if (differentialAgeAvailable) {
        logger() << differentialAge;
    } else {
        logger() << "unavailable";
    }
    logger() << " Diff station ID ";
    if (differentialStationIDAvailable) {
        logger() << differentialStationID;
    } else {
        logger() << "unavailable";
    }
    logger () << eol;
}

const char *SeaTalkParser::parseSignalQuality(uint8_t signalQualityCode,
                                              bool signalQualityAvailable) {
    if (signalQualityAvailable) {
        switch (signalQualityCode) {
            case 0:
                return "Fix Not Available";
            case 1:
                return "GPS Fix";
            case 2:
                return "Differential GPS Fix";
            case 3:
                return "PPS Fix";
            case 4:
                return "Real Time Kinematic";
            case 5:
                return "Float RTK";
            case 6:
                return "Estimated";
            case 7:
                return "Manual Input Mode";
            case 8:
                return "Simulated Mode";
            default:
                return "Unknown";
        }
    } else {
        return "Not Available";
    }
}

void SeaTalkParser::parseActiveSatellites(const SeaTalkLine &seaTalkLine) {
    etl::string<activeSatellitesLength + 1> idString;
    etl::string_stream idStream(idString);

    bool validIDFound = false;
    for (int pos = 2; pos <= 6; pos++) {
        if (seaTalkLine[pos] != 0) {
            if (validIDFound) {
                idStream << ",";
            } else {
                validIDFound = true;
            }
            idStream << seaTalkLine[pos];
        }
    }

    GPSData &gpsData = instrumentData.gpsData();
    gpsData.beginUpdates();
    gpsData.activeSatellitesLeaf = idString;
    gpsData.endUpdates();

    logger() << logDebugSeaTalk << "Active Satellites " << idString << eol;
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
        SeaTalkCommand command = seaTalkLine.command();
        logger() << logWarnSeaTalk << command << " message with an unexpected length (" << length
                 << "): " << seaTalkLine << eol;

        commandLengthErrors++;

        return false;
    }

    return true;
}

bool SeaTalkParser::checkAttribute(const SeaTalkLine &seaTalkLine, uint8_t expectedAttribute,
                                   uint8_t mask) {
    uint8_t attribute = seaTalkLine.attribute();

    if ((attribute & mask) != expectedAttribute) {
        SeaTalkCommand command = seaTalkLine.command();
        logger() << logWarnSeaTalk << "Unsupported "<< command << " attribute " << Hex << attribute
                 << ": " << seaTalkLine << eol;

        commandFormatErrors++;

        return false;
    }

    return true;
}

void SeaTalkParser::exportStats(uint32_t msElapsed) {
    ignoredCommandsLeaf = ignoredCommands;
    unknownCommandsLeaf = unknownCommands;
    commandLengthErrorsLeaf = commandLengthErrors;
    commandFormatErrorsLeaf = commandFormatErrors;
}

const char *SeaTalkParser::modeBitsToName(uint8_t modeBits) const {
    switch (modeBits) {
        case 0x0:
            return "Standby";
        case 0x2:
            return "Auto";
        case 0x6:
            return "Wind";
        case 0xa:
            return "Track";
        default:
            return "Unknown";
    }
}

void SeaTalkParser::coordinateToString(uint8_t degrees, uint16_t minutesX100, char suffix,
                                       etl::istring &string) const {
    etl::to_string(degrees, string);
    string.append("\xC2\xB0 ");
    etl::to_string(minutesX100, 2U, string, etl::format_spec().precision(2), true);
    string.append("' ");
    string.push_back(suffix);
}

void SeaTalkParser::rawCoordinateToString(uint8_t degrees, uint16_t minutesX1000, char suffix,
                                          etl::istring &string) const {
    etl::to_string(degrees, string);
    string.append("\xC2\xB0 ");
    etl::to_string(minutesX1000, 3U, string, etl::format_spec().precision(3), true);
    string.append("' ");
    string.push_back(suffix);
}
