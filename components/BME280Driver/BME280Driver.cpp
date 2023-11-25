/*
 * This file is part of LunaMon (https://github.com/LisaRowell/LunaMon)
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

#include "BME280Driver.h"
#include "I2CDriver.h"

#include "HundredthsInt16.h"
#include "HundredthsUInt8.h"
#include "HundredthsUInt32.h"
#include "Logger.h"

#include "esp_err.h"

#include <stdint.h>

#define BME280_DIG_T1_LSB_REG           0x88
#define BME280_DIG_T1_MSB_REG           0x89
#define BME280_DIG_T2_LSB_REG           0x8A
#define BME280_DIG_T2_MSB_REG           0x8B
#define BME280_DIG_T3_LSB_REG           0x8C
#define BME280_DIG_T3_MSB_REG           0x8D
#define BME280_DIG_P1_LSB_REG           0x8E
#define BME280_DIG_P1_MSB_REG           0x8F
#define BME280_DIG_P2_LSB_REG           0x90
#define BME280_DIG_P2_MSB_REG           0x91
#define BME280_DIG_P3_LSB_REG           0x92
#define BME280_DIG_P3_MSB_REG           0x93
#define BME280_DIG_P4_LSB_REG           0x94
#define BME280_DIG_P4_MSB_REG           0x95
#define BME280_DIG_P5_LSB_REG           0x96
#define BME280_DIG_P5_MSB_REG           0x97
#define BME280_DIG_P6_LSB_REG           0x98
#define BME280_DIG_P6_MSB_REG           0x99
#define BME280_DIG_P7_LSB_REG           0x9A
#define BME280_DIG_P7_MSB_REG           0x9B
#define BME280_DIG_P8_LSB_REG           0x9C
#define BME280_DIG_P8_MSB_REG           0x9D
#define BME280_DIG_P9_LSB_REG           0x9E
#define BME280_DIG_P9_MSB_REG           0x9F
#define BME280_DIG_H1_REG               0xA1
#define BME280_CHIP_ID_REG              0xD0 //Chip ID
#define BME280_RST_REG                  0xE0 //Softreset Reg
#define BME280_DIG_H2_LSB_REG           0xE1
#define BME280_DIG_H2_MSB_REG           0xE2
#define BME280_DIG_H3_REG               0xE3
#define BME280_DIG_H4_MSB_REG           0xE4
#define BME280_DIG_H4_LSB_REG           0xE5
#define BME280_DIG_H5_MSB_REG           0xE6
#define BME280_DIG_H6_REG               0xE7
#define BME280_CTRL_HUMIDITY_REG        0xF2 //Ctrl Humidity Reg
#define BME280_STAT_REG                 0xF3 //Status Reg
#define BME280_CTRL_MEAS_REG            0xF4 //Ctrl Measure Reg
#define BME280_CONFIG_REG               0xF5 //Configuration Reg
#define BME280_MEASUREMENTS_REG         0xF7 //Measurements register start
#define BME280_PRESSURE_MSB_REG         0xF7 //Pressure MSB
#define BME280_PRESSURE_LSB_REG         0xF8 //Pressure LSB
#define BME280_PRESSURE_XLSB_REG        0xF9 //Pressure XLSB
#define BME280_TEMPERATURE_MSB_REG      0xFA //Temperature MSB
#define BME280_TEMPERATURE_LSB_REG      0xFB //Temperature LSB
#define BME280_TEMPERATURE_XLSB_REG     0xFC //Temperature XLSB
#define BME280_HUMIDITY_MSB_REG         0xFD //Humidity MSB
#define BME280_HUMIDITY_LSB_REG         0xFE //Humidity LSB

#define BME280_CHIP_ID      0x60

#define BME280_STANDBY_PT5MS    0b000
#define BME280_STANDBY_62PT5MS  0b001
#define BME280_STANDBY_125MS    0b010
#define BME280_STANDBY_250MS    0b011
#define BME280_STANDBY_500MS    0b100
#define BME280_STANDBY_1S       0b101
#define BME280_STANDBY_10MS     0b110
#define BME280_STANDBY_20MS     0b111

#define BME280_FILTER_OFF       0b000
#define BME280_FILTER_2         0b001
#define BME280_FILTER_4         0b010
#define BME280_FILTER_8         0b011
#define BME280_FILTER_16        0b100

#define BME280_CONFIG_STANDBY_MASK  0b11100000
#define BME280_CONFIG_STANDBY_SHIFT 5
#define BME280_CONFIG_FILTER_MASK   0b00011100
#define BME280_CONFIG_FILTER_SHIFT  2

#define BME280_HUMIDITY_OFF             0b000
#define BME280_HUMIDITY_OVERSAMPLE_X1   0b001
#define BME280_HUMIDITY_OVERSAMPLE_X2   0b010
#define BME280_HUMIDITY_OVERSAMPLE_X4   0b011
#define BME280_HUMIDITY_OVERSAMPLE_X8   0b100
#define BME280_HUMIDITY_OVERSAMPLE_X16  0b101

#define BME280_CTRL_HUM_HUMIDITY_OVERSAMPLE_MASK  0b00000111
#define BME280_CTRL_HUM_HUMIDITY_OVERSAMPLE_SHIFT 0

#define BME280_TEMPERATURE_OFF              0b000
#define BME280_TEMPERATURE_OVERSAMPLE_X1    0b001
#define BME280_TEMPERATURE_OVERSAMPLE_X2    0b010
#define BME280_TEMPERATURE_OVERSAMPLE_X4    0b011
#define BME280_TEMPERATURE_OVERSAMPLE_X8    0b100
#define BME280_TEMPERATURE_OVERSAMPLE_X16   0b101

#define BME280_PRESSURE_OFF             0b000
#define BME280_PRESSURE_OVERSAMPLE_X1   0b001
#define BME280_PRESSURE_OVERSAMPLE_X2   0b010
#define BME280_PRESSURE_OVERSAMPLE_X4   0b011
#define BME280_PRESSURE_OVERSAMPLE_X8   0b100
#define BME280_PRESSURE_OVERSAMPLE_X16  0b101

#define BME280_MODE_SLEEP   0b00
#define BME280_MODE_FORCED  0b01
#define BME280_MODE_NORMAL  0b11

#define BME280_CTRL_MEAS_TEMPERATURE_OVERSAMPLE_MASK   0b11100000
#define BME280_CTRL_MEAS_TEMPERATURE_OVERSAMPLE_SHIFT  5
#define BME280_CTRL_MEAS_PRESSURE_OVERSAMPLE_MASK   0b00011100
#define BME280_CTRL_MEAS_PRESSURE_OVERSAMPLE_SHIFT  2
#define BME280_CTRL_MEAS_MODE_MASK                  0b00000011
#define BME280_CTRL_MEAS_MODE_SHIFT                 0

BME280Driver::BME280Driver(I2CMaster &ic2Master, uint8_t deviceAddr)
    : I2CDriver(ic2Master, deviceAddr) {
}

bool BME280Driver::detect() {
    // Verify that the sensor is there and that it's what we think it is.
    uint8_t id;
    try {
        id = readRegister(BME280_CHIP_ID_REG);
    }
    catch (esp_err_t error) {
        logger() << logErrorBME280Driver << "Failed to read BME280 Device ID." << eol;
        return false;
    }

    if (id == BME280_CHIP_ID) {
        logger() << logDebugBME280Driver << "Successfully read BME280 chip id." << eol;
        return true;
    } else {
        logger() << logErrorBME280Driver << "Unexpected BME280 chip id: " << Hex << id << eol;
        return false;
    }
}

void BME280Driver::start() {
    readCalibrationData();

	setStandbyTime(BME280_STANDBY_500MS);
	setFilter(BME280_FILTER_2);
    setHumidityOverSampling(BME280_HUMIDITY_OVERSAMPLE_X1);
    setPressureOverSampling(BME280_PRESSURE_OVERSAMPLE_X1);
	setTemperatureOverSampling(BME280_TEMPERATURE_OVERSAMPLE_X1);

	setMode(BME280_MODE_NORMAL);
}

void BME280Driver::readCalibrationData() {
	dig_T1 = (uint16_t)((readRegister(BME280_DIG_T1_MSB_REG) << 8) +
                        readRegister(BME280_DIG_T1_LSB_REG));
	dig_T2 = (int16_t)((readRegister(BME280_DIG_T2_MSB_REG) << 8) +
                       readRegister(BME280_DIG_T2_LSB_REG));
	dig_T3 = (int16_t)((readRegister(BME280_DIG_T3_MSB_REG) << 8) +
                       readRegister(BME280_DIG_T3_LSB_REG));

	dig_P1 = (uint16_t)((readRegister(BME280_DIG_P1_MSB_REG) << 8) +
                        readRegister(BME280_DIG_P1_LSB_REG));
	dig_P2 = (int16_t)((readRegister(BME280_DIG_P2_MSB_REG) << 8) +
                       readRegister(BME280_DIG_P2_LSB_REG));
	dig_P3 = (int16_t)((readRegister(BME280_DIG_P3_MSB_REG) << 8) +
                       readRegister(BME280_DIG_P3_LSB_REG));
	dig_P4 = (int16_t)((readRegister(BME280_DIG_P4_MSB_REG) << 8) +
                       readRegister(BME280_DIG_P4_LSB_REG));
	dig_P5 = (int16_t)((readRegister(BME280_DIG_P5_MSB_REG) << 8) +
                       readRegister(BME280_DIG_P5_LSB_REG));
	dig_P6 = (int16_t)((readRegister(BME280_DIG_P6_MSB_REG) << 8) +
                       readRegister(BME280_DIG_P6_LSB_REG));
	dig_P7 = (int16_t)((readRegister(BME280_DIG_P7_MSB_REG) << 8) +
                       readRegister(BME280_DIG_P7_LSB_REG));
	dig_P8 = (int16_t)((readRegister(BME280_DIG_P8_MSB_REG) << 8) +
                       readRegister(BME280_DIG_P8_LSB_REG));
	dig_P9 = (int16_t)((readRegister(BME280_DIG_P9_MSB_REG) << 8) +
                       readRegister(BME280_DIG_P9_LSB_REG));

	dig_H1 = (uint8_t)(readRegister(BME280_DIG_H1_REG));
	dig_H2 = (int16_t)((readRegister(BME280_DIG_H2_MSB_REG) << 8) +
                       readRegister(BME280_DIG_H2_LSB_REG));
	dig_H3 = (uint8_t)readRegister(BME280_DIG_H3_REG);
	dig_H4 = (int16_t)((readRegister(BME280_DIG_H4_MSB_REG) << 4) +
                       (readRegister(BME280_DIG_H4_LSB_REG) & 0x0F));
	dig_H5 = (int16_t)((readRegister(BME280_DIG_H5_MSB_REG) << 4) +
                       ((readRegister(BME280_DIG_H4_LSB_REG) >> 4) & 0x0F));
	dig_H6 = (int8_t)readRegister(BME280_DIG_H6_REG);
}

void BME280Driver::setStandbyTime(uint8_t standByCode) {
	if (standByCode > 0b111) {
        standByCode = 0; //Error check. Default to 0.5ms
    }

	uint8_t controlData = readRegister(BME280_CONFIG_REG);
    controlData &= ~BME280_CONFIG_STANDBY_MASK;
	controlData |= standByCode << BME280_CONFIG_STANDBY_SHIFT;
	writeRegister(BME280_CONFIG_REG, controlData);
}

void BME280Driver::setFilter(uint8_t filterCode) {
	if (filterCode > BME280_CONFIG_FILTER_MASK) {
        filterCode = 0; //Error check. Default to filter off
    }

	uint8_t controlData = readRegister(BME280_CONFIG_REG);
    controlData &= ~BME280_CONFIG_FILTER_MASK;
	controlData |= filterCode << BME280_CONFIG_FILTER_SHIFT;
	writeRegister(BME280_CONFIG_REG, controlData);
}

void BME280Driver::setHumidityOverSampling(uint8_t humidtyOversamplingCode) {
    if (humidtyOversamplingCode > BME280_CTRL_HUM_HUMIDITY_OVERSAMPLE_MASK) {
        humidtyOversamplingCode = BME280_HUMIDITY_OVERSAMPLE_X1;
    }

	uint8_t controlData = readRegister(BME280_CTRL_HUMIDITY_REG);
	controlData &= ~BME280_CTRL_HUM_HUMIDITY_OVERSAMPLE_MASK;
	controlData |= humidtyOversamplingCode << BME280_CTRL_HUM_HUMIDITY_OVERSAMPLE_SHIFT;
	writeRegister(BME280_CTRL_HUMIDITY_REG, controlData);
}

void BME280Driver::setTemperatureOverSampling(uint8_t temperatureOversamplingCode) {
    if (temperatureOversamplingCode > BME280_CTRL_MEAS_TEMPERATURE_OVERSAMPLE_MASK) {
        temperatureOversamplingCode = BME280_TEMPERATURE_OVERSAMPLE_X1;
    }

	uint8_t controlData = readRegister(BME280_CTRL_MEAS_REG);
	controlData &= ~BME280_CTRL_MEAS_TEMPERATURE_OVERSAMPLE_MASK;
	controlData |= temperatureOversamplingCode << BME280_CTRL_MEAS_TEMPERATURE_OVERSAMPLE_SHIFT;
	writeRegister(BME280_CTRL_MEAS_REG, controlData);
}

void BME280Driver::setPressureOverSampling(uint8_t pressureOversamplingCode) {
    if (pressureOversamplingCode > BME280_CTRL_MEAS_PRESSURE_OVERSAMPLE_MASK) {
        pressureOversamplingCode = BME280_PRESSURE_OVERSAMPLE_X1;
    }

	uint8_t controlData = readRegister(BME280_CTRL_MEAS_REG);
	controlData &= ~BME280_CTRL_MEAS_PRESSURE_OVERSAMPLE_MASK;
	controlData |= pressureOversamplingCode << BME280_CTRL_MEAS_PRESSURE_OVERSAMPLE_SHIFT;
	writeRegister(BME280_CTRL_MEAS_REG, controlData);
}

void BME280Driver::setMode(uint8_t modeCode) {
	if (modeCode > BME280_CTRL_MEAS_MODE_MASK) {
        modeCode = BME280_MODE_SLEEP;
    }
	
	uint8_t controlData = readRegister(BME280_CTRL_MEAS_REG);
	controlData &= ~BME280_CTRL_MEAS_MODE_MASK;
	controlData |= modeCode << BME280_CTRL_MEAS_MODE_SHIFT;
	writeRegister(BME280_CTRL_MEAS_REG, controlData);
}

void BME280Driver::read(HundredthsInt16 &temperatureC, HundredthsUInt32 &pressureMBar,
                        HundredthsUInt8 &relativeHumidity) {
	uint8_t data[8];
	readRegisters(BME280_MEASUREMENTS_REG, data, 8);

    int32_t t_fine;
    int32_t adcTemperature = ((uint32_t)data[3] << 12) | ((uint32_t)data[4] << 4) |
                             ((data[5] >> 4) & 0x0F);
    int32_t temperatureHundredthsC = compensateTemperature(adcTemperature, t_fine);
    temperatureC.setFromHundredths(temperatureHundredthsC);

    int32_t adc_P = ((uint32_t)data[0] << 12) | ((uint32_t)data[1] << 4) |
                    ((data[2] >> 4) & 0x0F);
    uint32_t pressureQ24Dot8 = compensatePressure(adc_P, t_fine);
    HundredthsUInt32 pressurePa;
    pressurePa.setFromQ24Dot8(pressureQ24Dot8);
    pressureMBar = pressurePa / 100;

    int32_t adcHumidity = ((uint32_t)data[6] << 8) | ((uint32_t)data[7]);
    uint32_t relativeHumidityQ22Dot10 = compensateHumidity(adcHumidity, t_fine);
    relativeHumidity.setFromQ22Dot10(relativeHumidityQ22Dot10);
}

// From datasheet
// Returns temperature in DegC, resolution is 0.01 DegC. Output value of “5123” equals 51.23 DegC.
// t_fine carries fine temperature which is used in pressure and humidity compensation
int32_t BME280Driver::compensateTemperature(int32_t adc_T, int32_t &t_fine) {
    int32_t var1, var2, T;
    var1 = ((((adc_T >> 3) - ((int32_t)dig_T1 << 1))) * ((int32_t)dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((int32_t)dig_T1)) * ((adc_T>>4) -
            ((int32_t)dig_T1))) >> 12) * ((int32_t)dig_T3)) >> 14;
    t_fine = var1 + var2;
    T = (t_fine * 5 + 128) >> 8;

    return T;
}

// From datasheet
// Returns pressure in Pa as unsigned 32 bit integer in Q24.8 format (24 integer bits and 8
// fractional bits). Output value of “24674867” represents 24674867/256 = 96386.2 Pa = 963.862 hPa
uint32_t BME280Driver::compensatePressure(int32_t adc_P, int32_t t_fine) {
    int64_t var1, var2, p;

    var1 = ((int64_t)t_fine) - 128000;
    var2 = var1 * var1 * (int64_t)dig_P6;
    var2 = var2 + ((var1 * (int64_t)dig_P5) << 17);
    var2 = var2 + (((int64_t)dig_P4) << 35);
    var1 = ((var1 * var1 * (int64_t)dig_P3) >> 8) + ((var1 * (int64_t)dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)dig_P1) >> 33;
    if (var1 == 0) {
        return 0; // avoid exception caused by division by zero
    }
    p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((int64_t)dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)dig_P8) * p) >> 19;
    p = ((p + var1 + var2) >> 8) + (((int64_t)dig_P7) << 4);

    return (uint32_t)p;
}

// From datasheet
// Returns humidity in %RH as unsigned 32 bit integer in Q22.10 format (22 integer and 10 fractional
// bits). Output value of “47445” represents 47445/1024 = 46.333 %RH
uint32_t BME280Driver::compensateHumidity(int32_t adc_H, int32_t t_fine) {
    int32_t v_x1_u32r;
    v_x1_u32r = (t_fine - ((int32_t)76800));
    v_x1_u32r = (((((adc_H << 14) - (((int32_t)dig_H4) << 20) - (((int32_t)dig_H5) * v_x1_u32r)) +
                  ((int32_t)16384)) >> 15) *
                  (((((((v_x1_u32r * ((int32_t)dig_H6)) >> 10) *
                  (((v_x1_u32r * ((int32_t)dig_H3)) >> 11) + ((int32_t)32768))) >> 10) +
                  ((int32_t)2097152)) * ((int32_t)dig_H2) + 8192) >> 14));
    v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) *
                ((int32_t)dig_H1)) >> 4));
    v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
    v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
    return (uint32_t)(v_x1_u32r >> 12);
}
