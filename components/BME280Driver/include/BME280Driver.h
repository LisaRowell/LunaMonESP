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

class I2CMaster;

#include <stdint.h>
#include <stddef.h>

class BME280Driver {
    private:
        I2CMaster &ic2Master;
        uint8_t deviceAddr;

    	uint16_t dig_T1;
        int16_t dig_T2;
        int16_t dig_T3;

        uint16_t dig_P1;
        int16_t dig_P2;
        int16_t dig_P3;
        int16_t dig_P4;
        int16_t dig_P5;
        int16_t dig_P6;
        int16_t dig_P7;
        int16_t dig_P8;
        int16_t dig_P9;

        uint8_t dig_H1;
        int16_t dig_H2;
        uint8_t dig_H3;
        int16_t dig_H4;
        int16_t dig_H5;
        int8_t dig_H6;

        uint8_t readRegister(uint8_t offset);
        void writeRegister(uint8_t offset, uint8_t value);
        void readRegisters(uint8_t offset, uint8_t *registers, size_t count);
        void readCalibrationData();
        void setStandbyTime(uint8_t standByCode);
        void setFilter(uint8_t filterCode);
        void setHumidityOverSampling(uint8_t humidtyOversamplingCode);
        void setTemperatureOverSampling(uint8_t temperatureOversamplingCode);
        void setPressureOverSampling(uint8_t pressureOversamplingCode);
        void setMode(uint8_t modeCode);
        int32_t compensateTemperature(int32_t adc_T, int32_t &t_fine);
        uint32_t compensatePressure(int32_t adc_P, int32_t t_fine);
        uint32_t compensateHumidity(int32_t adc_H, int32_t t_fine);

    public:
        BME280Driver(I2CMaster &ic2Master, uint8_t deviceAddr);
        bool detect();
        void start();
        void read(int32_t &temperatureHundredthsC, uint32_t &pressureHundredthsMBar,
                  uint32_t &relativeHumidityHundredths);
};
