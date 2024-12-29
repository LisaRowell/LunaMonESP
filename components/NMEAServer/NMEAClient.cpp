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

#include "NMEAClient.h"

#include "NMEALine.h"

#include "DataModelStringLeaf.h"
#include "Logger.h"

#include "etl/string.h"
#include "etl/to_string.h"

#include <arpa/inet.h>
#include <lwip/sockets.h>

#include <stddef.h>
#include <stdint.h>

NMEAClient::NMEAClient(int socket, struct sockaddr_in &sourceAddr)
    : socket(socket),
      sourceAddr(sourceAddr) {
    // We want client sockets to be non-blocking so that we don't lose input messages because of a
    // slow client or one that is going away.
    int nonblocking = 1;
    lwip_ioctl(socket, FIONBIO, &nonblocking);
    setSocketKeepalive();
}

bool NMEAClient::sendLine(const NMEALine &inputLine, bool &dropped) {
    ssize_t result = send(socket, inputLine.data(), inputLine.length(), 0);
    if (result < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            // The socket buffer is full so the client isn't going to get this message. It's still
            // open though so we don't disconnect.
            logger() << logDebugNMEAServer << "Send to NMEA Server client " << *this
                     << " failed due to full buffer (" << strerror(errno) << ")" << eol;
            dropped = true;
            return true;
        } else {
            logger() << logWarnNMEAServer << "Send to NMEA Server client " << sourceAddr
                     << " failed: " << strerror(errno) << "(" << errno << ")" << eol;
            dropped = true;
            return false;
        }
    } else if ((size_t)result != inputLine.length()) {
        // Is this a thing?
        logger() << logWarnNMEAServer << "Partial NMEA message sent to client " << sourceAddr
                 << eol;
        dropped = false;
        return true;
    } else {
        dropped = false;
        return true;
    }
}

void NMEAClient::setSocketKeepalive() {
    int keepAlive = 1;
    int keepIdle = CONFIG_LUNAMON_NMEA_SERVER_TCP_KEEPALIVE_IDLE;
    int keepInterval = CONFIG_LUNAMON_NMEA_SERVER_TCP_KEEPALIVE_INTERVAL;
    int keepCount = CONFIG_LUNAMON_NMEA_SERVER_TCP_KEEPALIVE_COUNT;

    setsockopt(socket, SOL_SOCKET, SO_KEEPALIVE, &keepAlive, sizeof(int));
    setsockopt(socket, IPPROTO_TCP, TCP_KEEPIDLE, &keepIdle, sizeof(int));
    setsockopt(socket, IPPROTO_TCP, TCP_KEEPINTVL, &keepInterval, sizeof(int));
    setsockopt(socket, IPPROTO_TCP, TCP_KEEPCNT, &keepCount, sizeof(int));
}

void NMEAClient::setNameLeaf(DataModelStringLeaf *nameLeaf) {
    char nameBuffer[25];
    inet_ntoa_r(sourceAddr.sin_addr.s_addr, nameBuffer, sizeof(nameBuffer) - 1);

    etl::string_ext nameStr(nameBuffer, nameBuffer, sizeof(nameBuffer));
    nameStr.append(":");
    etl::to_string(ntohs(sourceAddr.sin_port), nameStr, true);

    *nameLeaf = nameStr;
}

NMEAClient::~NMEAClient() {
    shutdown(socket, SHUT_RDWR);
    close(socket);
}

Logger & operator << (Logger &logger, const NMEAClient &client) {
    char addrStr[16];
    inet_ntoa_r(client.sourceAddr.sin_addr.s_addr, addrStr, sizeof(addrStr) - 1);

    logger << addrStr << ":" << ntohs(client.sourceAddr.sin_port);

    return logger;
}
