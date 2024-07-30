/*
 * This file is part of LunaMon (https://github.com/LisaRowell/LunaMonESP)
 * Copyright (C) 2021-2024 Lisa Rowell
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

#include "StatsManager.h"

#include "DataModel.h"
#include "DataModelStringLeaf.h"
#include "DataModelUInt32Leaf.h"

#include "AISContacts.h"

#include "WiFiManager.h"
#include "MQTTBroker.h"
#include "NMEA.h"
#include "NMEADataModelBridge.h"
#include "LogManager.h"
#include "Logger.h"

#include "etl/string.h"

class NMEAWiFiSource;
class NMEAUARTSource;
class StatusLED;
class I2CMaster;
class EnvironmentalMon;

class LunaMon {
    private:
        StatsManager statsManager;
        DataModel dataModel;
        AISContacts aisContacts;
        WiFiManager wifiManager;
        MQTTBroker mqttBroker;
        NMEA nmea;
        NMEADataModelBridge nmeaDataModelBridge;
        LogManager logManager;
        Logger logger;
        NMEAWiFiSource *nmeaWiFiSource;
        NMEAUARTSource *nmeaUART1Source;
        NMEAUARTSource *nmeaUART2Source;
        I2CMaster *ic2Master;
        EnvironmentalMon *environmentalMon;
        StatusLED *statusLED;

        etl::string<10> versionBuffer;
        DataModelStringLeaf versionLeaf;
        DataModelUInt32Leaf uptimeLeaf;

        void initNVS();

    public:
        LunaMon();
        void run();
};

#endif // LUNA_MON_H
