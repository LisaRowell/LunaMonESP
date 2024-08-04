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

#ifndef NMEA_MESSAGE_BUFFER_H
#define NMEA_MESSAGE_BUFFER_H

#include "NMEADBKMessage.h"
#include "NMEADBSMessage.h"
#include "NMEADBTMessage.h"
#include "NMEADPTMessage.h"
#include "NMEAGGAMessage.h"
#include "NMEAGLLMessage.h"
#include "NMEAGSAMessage.h"
#include "NMEAGSTMessage.h"
#include "NMEAGSVMessage.h"
#include "NMEAHDGMessage.h"
#include "NMEAMTWMessage.h"
#include "NMEAMWVMessage.h"
#include "NMEARMCMessage.h"
#include "NMEARSAMessage.h"
#include "NMEATXTMessage.h"
#include "NMEAVDMMessage.h"
#include "NMEAVDOMessage.h"
#include "NMEAVHWMessage.h"
#include "NMEAVTGMessage.h"

#include "etl/algorithm.h"

#include <stddef.h>

constexpr size_t NMEA_MESSAGE_BUFFER_SIZE = etl::multimax(sizeof(NMEADBKMessage),
                                                          sizeof(NMEADBSMessage),
                                                          sizeof(NMEADBTMessage),
                                                          sizeof(NMEADPTMessage),
                                                          sizeof(NMEAGGAMessage),
                                                          sizeof(NMEAGSAMessage),
                                                          sizeof(NMEAGLLMessage),
                                                          sizeof(NMEAGSTMessage),
                                                          sizeof(NMEAGSVMessage),
                                                          sizeof(NMEAHDGMessage),
                                                          sizeof(NMEAMTWMessage),
                                                          sizeof(NMEAMWVMessage),
                                                          sizeof(NMEARMCMessage),
                                                          sizeof(NMEARSAMessage),
                                                          sizeof(NMEATXTMessage),
                                                          sizeof(NMEAVDMMessage),
                                                          sizeof(NMEAVDOMessage),
                                                          sizeof(NMEAVHWMessage),
                                                          sizeof(NMEAVTGMessage));


#endif
