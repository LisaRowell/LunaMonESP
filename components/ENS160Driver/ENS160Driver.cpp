/*
 * This file is part of LunaMon (https://github.com/LisaRowell/LunaMonESP)
 * Copyright (C) 2023 Lisa Rowell
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

#include "ENS160Driver.h"

#include "I2CDriver.h"
#include "Logger.h"

#include "esp_err.h"

#include <stdint.h>

#define ENS160_PART_ID_REG          0x00
#define ENS160_OPMODE_REG           0x10
#define ENS160_CONFIG_REG           0x11
#define ENS160_COMMAND_REG          0x12
#define ENS160_TEMP_IN_REG          0x13
#define ENS160_RH_IN_REG            0x15
#define ENS160_DEVICE_STATUS_REG    0x20
#define ENS160_DATA_AQI_REG         0x21
#define ENS160_DATA_TVOC_REG        0x22
#define ENS160_DATA_ECO2_REG        0x24
#define ENS160_DATA_T_REG           0x30
#define ENS160_DATA_RH_REG          0x32
#define ENS160_DATA_MISR_REG        0x38
#define ENS160_GPR_WRITE_REG        0x40
#define ENS160_GPR_READ_REG         0x48

#define ENS160_PART_ID  0x0160

#define ENS160_OPMODE_DEEP_SLEEP    0x00
#define ENS160_OPMODE_IDLE          0x01
#define ENS160_OPMODE_STANDARD      0x02
#define ENS160_OPMODE_RESET         0xF0

#define ENS160_CONFIG_INTPOL_MASK           0b01000000
#define ENS160_CONFIG_INTPOL_SHIFT          6
#define ENS160_CONFIG_INTPOL_ACTIVE_LOW     0
#define ENS160_CONFIG_INTPOL_ACTIVE_HI      1
#define ENS160_CONFIG_INT_CFG_MASK          0b00100000
#define ENS160_CONFIG_INT_CFG_SHIFT         5
#define ENS160_CONFIG_INT_CFG_OPEN_DRAIN    0
#define ENS160_CONFIG_INT_CFG_PUSH_PULL     1
#define ENS160_CONFIG_INTGPR_MASK           0b00001000
#define ENS160_CONFIG_INTGPR_SHIFT          3
#define ENS160_CONFIG_INTDAT_MASK           0b00000010
#define ENS160_CONFIG_INTDAT_SHIFT          1
#define ENS160_CONFIG_INTEN_MASK            0b00000001
#define ENS160_CONFIG_INTEN_SHIFT           0

#define ENS160_COMMAND_NOP          0x00
#define ENS160_COMMAND_GET_APPVER   0x0E
#define ENS160_COMMAND_CLRGPR       0xCC

#define ENS160_STATUS_STATAS_MASK       0b10000000
#define ENS160_STATUS_STATER_MASK       0b01000000
#define ENS160_STATUS_VALIDITY_MASK     0b00001100
#define ENS160_STATUS_VALIDITY_SHIFT    2
#define ENS160_STATUS_NEWDAT_MASK       0b00000010
#define ENS160_STATUS_NEWGPR_MASK       0b00000001

#define ENS160_VALIDITY_NORMAL_OPERATION        0
#define ENS160_VALIDITY_WARM_UP_PHASE           1
#define ENS160_VALIDITY_INITIAL_START_UP_PHASE  2
#define ENS160_VALIDITY_INVALID_OUTPUT          3

ENS160Driver::ENS160Driver(I2CMaster &ic2Master, uint8_t deviceAddr)
    : I2CDriver(ic2Master, deviceAddr) {
}

bool ENS160Driver::detect() {
    // Verify that the sensor is there and that it's what we think it is.
    uint8_t idBytes[2];
    try {
        readRegisters(ENS160_PART_ID_REG, idBytes, 2);
    }
    catch (esp_err_t error) {
        logger() << logErrorENS160Driver << "Failed to read ENS160 Device ID." << eol;
        return false;
    }

    const uint16_t id = idBytes[1] << 8 | idBytes[0];
    if (id == ENS160_PART_ID) {
        logger() << logErrorENS160Driver << "Successfully read ENS160 chip id." << eol;
        return true;
    } else {
        logger() << logErrorENS160Driver << "Unexpected ENS160 chip id: " << Hex << id << eol;
        return false;
    }
}

void ENS160Driver::start() {
    writeRegister(ENS160_CONFIG_REG, 0);
    writeRegister(ENS160_OPMODE_REG, ENS160_OPMODE_STANDARD);

    logger() << logDebugENS160Driver << "Set Op Mode to Standard." << eol;
}

bool ENS160Driver::read(bool &startingUp, uint8_t &aqi, uint16_t &tvoc, uint16_t &eco2) {
    // We read in the Status (1 byte), AQI (1 byte), TVOC (2 bytes), and eCO2 (2 bytes) in one burst
    // read.
    uint8_t registers[6];
    readRegisters(ENS160_DEVICE_STATUS_REG, registers, 6);

    const uint8_t status = registers[0];
    logStatus(status);

    aqi = registers[1];
    tvoc = registers[2] | registers[3] << 8;
    eco2 = registers[4] | registers[5] << 8;

    const uint8_t validity = (status & ENS160_STATUS_VALIDITY_MASK) >> ENS160_STATUS_VALIDITY_SHIFT;
    switch (validity) {
        case ENS160_VALIDITY_NORMAL_OPERATION:
            startingUp = false;
            break;

        case ENS160_VALIDITY_WARM_UP_PHASE:
        case ENS160_VALIDITY_INITIAL_START_UP_PHASE:
            startingUp = true;
            break;

        case ENS160_VALIDITY_INVALID_OUTPUT:
            startingUp = false;
            return false;
    }

    return (status & ENS160_STATUS_STATAS_MASK) && !(status & ENS160_STATUS_STATER_MASK); 
}

void ENS160Driver::logStatus(uint8_t status) {
    logger() << logDebugENS160Driver << "Status: ";
    if (status & ENS160_STATUS_STATAS_MASK) {
        logger() << "Running";
    } else {
        logger() << "Not Running";
    }
    if (status & ENS160_STATUS_STATER_MASK) {
        logger() << ", Error";
    } else {
        logger() << ", No Error";
    }
 
    const uint8_t validity = (status & ENS160_STATUS_VALIDITY_MASK) >> ENS160_STATUS_VALIDITY_SHIFT;
    switch (validity) {
        case ENS160_VALIDITY_NORMAL_OPERATION:
            logger() << ", Normal Operation";
            break;
        case ENS160_VALIDITY_WARM_UP_PHASE:
            logger() << ", Warm Up Phase";
            break;
        case ENS160_VALIDITY_INITIAL_START_UP_PHASE:
            logger() << ", Initial Start Up Phase";
            break;
        case ENS160_VALIDITY_INVALID_OUTPUT:
            logger() << ", Invalid Output";
            break;
    }

    if (status & ENS160_STATUS_NEWDAT_MASK) {
        logger() << ", New Data";
    } else {
        logger() << ", No New Data";
    }
    if (status & ENS160_STATUS_NEWGPR_MASK) {
        logger() << ", New GPR Data";
    } else {
        logger() << ", No New GPR Data";
    }
    logger() << eol;
}

const char *ENS160Driver::aqiDescription(uint8_t aqi) {
    switch (aqi) {
        case 1:
            return "Excellent";
        case 2:
            return "Good";
        case 3:
            return "Moderate";
        case 4:
            return "Poor";
        case 5:
            return "Unhealthy";
        default:
            return "Invalid";
    }
}

const char *ENS160Driver::tvocDescription(uint16_t tvoc) {
    if (tvoc < 300) {
        return "Low";
    } else if (tvoc < 500) {
        return "Acceptable";
    } else if (tvoc < 1000) {
        return "Marginal";
    } else if (tvoc < 3000) {
        return "High";
    } else {
        return "Very High";
    }
}

const char *ENS160Driver::eco2Description(uint16_t eco2) {
    if (eco2 < 250) {
        return "Abnormally Low";
    } else if (eco2 < 400) {
        return "Excellent";
    } else if (eco2 < 1000) {
        return "Good";
    } else if (eco2 < 2000) {
        return "Poor";
    } else if (eco2 < 5000) {
        return "Bad";
    } else if (eco2 < 40000) {
        return "Dangerous";
    } else {
        return "Highly Dangerous";
    }
}
