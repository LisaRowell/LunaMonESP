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

#ifndef GPS_BRIDGE_H
#define GPS_BRIDGE_H

class InstrumentData;
class GPSData;
class NMEAGGAMessage;
class NMEAGLLMessage;
class NMEAGSAMessage;
class NMEAGSTMessage;
class NMEARMCMessage;
class NMEAVTGMessage;

#include <stddef.h>

class GPSBridge {
    private:
        static constexpr size_t activeSatellitesLength = 72;

        GPSData &gpsData;

    public:
        GPSBridge(InstrumentData &instrumentData);
        void bridgeNMEAGGAMessage(const NMEAGGAMessage *message);
        void bridgeNMEAGLLMessage(const NMEAGLLMessage *message);
        void bridgeNMEAGSAMessage(const NMEAGSAMessage *message);
        void bridgeNMEAGSTMessage(const NMEAGSTMessage *message);
        void bridgeNMEARMCMessage(const NMEARMCMessage *message);
        void bridgeNMEAVTGMessage(const NMEAVTGMessage *message);
};

#endif // GPS_BRIDGE_H
