/*
 * This file is part of LunaMon (https://github.com/LisaRowell/LunaMonESP)
 * Copyright (C) 2021-2023 Lisa Rowell
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

#ifndef LUNA_MON_H
#define LUNA_MON_H

#include "DataModel.h"
#include "WiFiManager.h"
#include "MQTTBroker.h"
#include "Logger.h"

class NMEAWiFiSource;
class StatusLED;
class I2CMaster;
class EnvironmentalMon;

class LunaMon {
    private:
        DataModel dataModel;
        WiFiManager wifiManager;
        MQTTBroker mqttBroker;
        Logger logger;
        NMEAWiFiSource *nmeaWiFiSource;
        I2CMaster *ic2Master;
        EnvironmentalMon *environmentalMon;
        StatusLED *statusLED;

        void initNVS();

    public:
        LunaMon();
        void run();
};

#endif // LUNA_MON_H
