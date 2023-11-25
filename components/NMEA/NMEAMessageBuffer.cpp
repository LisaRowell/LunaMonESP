/*
 * This file is part of LunaMon (https://github.com/LisaRowell/LunaMon)
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

#include "NMEAMessageBuffer.h"

#include "NMEAGGAMessage.h"
#include "NMEAGLLMessage.h"
#include "NMEAGSAMessage.h"
#include "NMEAGSTMessage.h"
#include "NMEAGSVMessage.h"
#include "NMEARMCMessage.h"
#include "NMEATXTMessage.h"
#include "NMEAVDMVDOMessage.h"
#include "NMEAVTGMessage.h"

#include <stdint.h>

#define MAX(a,b) (((a)>(b))?(a):(b))

#define NMEA_MESSAGE_BUFFER_SIZE \
(MAX(MAX(MAX(MAX(sizeof(NMEAGGAMessage), sizeof(NMEAGSAMessage)), \
             MAX(sizeof(NMEAGLLMessage), sizeof(NMEAGSTMessage))), \
         MAX(MAX(sizeof(NMEAGSVMessage), sizeof(NMEARMCMessage)), \
             MAX(sizeof(NMEATXTMessage), sizeof(NMEAVDMVDOMessage)))), \
     sizeof(NMEAVTGMessage)))

uint8_t nmeaMessageBuffer[NMEA_MESSAGE_BUFFER_SIZE];
