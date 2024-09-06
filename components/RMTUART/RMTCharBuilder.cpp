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

#include "RMTCharBuilder.h"
#include "InterfaceParams.h"

#include "Logger.h"
#include "Error.h"

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

#include <stdint.h>

RMTCharBuilder::RMTCharBuilder(InterfaceDataWidth dataWidth, InterfaceParity parity,
                               InterfaceStopBits stopBits, uint16_t bitDuration,
                               QueueHandle_t outputQueue)
    : bitDuration(bitDuration),
      bitDurationSlop(bitDuration / 4),
      parity(parity),
      outputQueue(outputQueue),
      state(SYCHRONIZING),
      bitsAccumulated(0),
      dataBits(0),
      frameErrors(0),
      glitchBits(0),
      shortStopBits(0),
      runOnFrames(0),
      wrongBitErrors(0),
      parityErrors(0),
      overrunErrors(0) {
    switch (dataWidth) {
        case InterfaceDataWidth::DATA_WIDTH_5_BITS:
            bitsPerChar = 5;
            break;
        case InterfaceDataWidth::DATA_WIDTH_6_BITS:
            bitsPerChar = 6;
            break;
        case InterfaceDataWidth::DATA_WIDTH_7_BITS:
            bitsPerChar = 7;
            break;
        case InterfaceDataWidth::DATA_WIDTH_8_BITS:
            bitsPerChar = 8;
            break;
        case InterfaceDataWidth::DATA_WIDTH_9_BITS:
            bitsPerChar = 9;
            break;
        default:
            logger() << logErrorRMTUART << "Bad data width (" << (uint8_t)dataWidth << ")" << eol;
            errorExit();
    }

    uint8_t halfStopBits;
    switch (stopBits) {
        case InterfaceStopBits::STOP_BITS_1:
            halfStopBits = 2;
            break;
        case InterfaceStopBits::STOP_BITS_1_5:
            halfStopBits = 3;
            break;
        case InterfaceStopBits::STOP_BITS_2:
            halfStopBits = 4;
            break;
        default:
            logger() << logErrorRMTUART << "Stop bit configuration (" << (uint8_t)stopBits << ")"
                     << eol;
            errorExit();
    }
      minStopBitDuration = (((bitDuration * halfStopBits) / 2) * 8) / 10;

    switch (parity) {
        case InterfaceParity::PARITY_NONE:
            dataBitsPerFrame = bitsPerChar;
            break;
        case InterfaceParity::PARITY_EVEN:
        case InterfaceParity::PARITY_ODD:
        case InterfaceParity::PARITY_MARK:
        case InterfaceParity::PARITY_SPACE:
            dataBitsPerFrame = bitsPerChar + 1;
            break;
        default:
            logger() << logErrorRMTUART << "Bad parity (" << (uint8_t)parity << ")" << eol;
            errorExit();
    }
}

void RMTCharBuilder::addBits(uint16_t duration, uint16_t level) {
    switch (state) {
        case WAITING_OUT_FIRST_BITS:
            break;

        case SYCHRONIZING:
            bitsReceivedWhileSynchronizing(duration, level);
            break;

        case START_OF_FRAME:
            addStartOfFrameBits(duration, level);
            break;

        case MID_FRAME_EXPECTING_1:
            addMidFrameBitsExpecting1(duration, level);
            break;

        case MID_FRAME_EXPECTING_0:
            addMidFrameBitsExpecting0(duration, level);
            break;

        case WAITING_FOR_STOP_BITS:
            stopBitsReceived(duration, level);
            break;

        case DISCARD_STREAM:
            break;
    }
}

void RMTCharBuilder::streamComplete() {
    switch (state) {
        case WAITING_OUT_FIRST_BITS:
            state = SYCHRONIZING;
            break;

        case SYCHRONIZING:
            // Probably not possible, it would mean a zero length stream from the RMT.
            break;

        case START_OF_FRAME:
            // I don't think we really will see this out of the RMT, with it instead ending the
            // sequence getting the stop bits, but it doesn't seem like a bad condition. Nothing
            // to do
            break;

        case MID_FRAME_EXPECTING_1:
            // This means that the last bits of the last character in the stream are 1, the same
            // level as the stop bit(s)
            addOneBits(dataBitsPerFrame - bitsAccumulated);
            state = START_OF_FRAME;
            break;

        case MID_FRAME_EXPECTING_0:
            // This would happen if the RMT got an idle stretch with a 0 value, which we shouldn't
            // see. In this case we should probably resynchronize...
            frameErrors++;
            state = SYCHRONIZING;
            break;

        case WAITING_FOR_STOP_BITS:
            // This should be the normal, happy end to a sequence of bits
            state = START_OF_FRAME;
            dataBitsCompleted();
            break;

        case DISCARD_STREAM:
            state = START_OF_FRAME;
            break;
    }

    bitsAccumulated = 0;
    dataBits = 0x0000;
}

void RMTCharBuilder::bitsReceivedWhileSynchronizing(uint16_t duration, uint16_t level) {
    // We really don't get crafty with this, later improvements make wish to. The current scheme is
    // to always toss the first stream of bits and all subsequent streams until we see one with a
    // leading 0 which would indicate a high of at least a frame in duration, followed by a start
    // bit. If we're in this state we've already skipped the first batch of bits.
    if (level == 0) {
        // Matches a start bit
        state = START_OF_FRAME;
        addStartOfFrameBits(duration, level);
    } else {
        // This shouldn't have happened?
        state = WAITING_OUT_FIRST_BITS;
    }
}

void RMTCharBuilder::addStartOfFrameBits(uint16_t duration, uint16_t level) {
    // Start bits should always be zero, if not it's likely crud at the start of some device
    // powering up. Discard the rest of the stream...
    if (level != 0) {
        logger() << logDebugRMTUART << "One bits at start of frame...discarding stream" << eol;
        state = DISCARD_STREAM;
        frameErrors++;
        return;
    }

    bitsAccumulated = 0;
    dataBits = 0;

    uint16_t fullBits = durationToFullBits(duration);
    if (fullBits == 0) {
        logger() << logDebugRMTUART << "Glitch bit at start of frame" << eol;
        state = DISCARD_STREAM;
        glitchBits++;
        return;
    }
    if (fullBits > dataBitsPerFrame + 1) {
        // We should never have more than dataBitsPerFrame + 1 zero bits in a row as this would mean
        // that we extended into the stop bit(s), which should be 1.
        logger() << logDebugRMTUART << "Too long of a duration at start of frame" << eol;
        state = DISCARD_STREAM;
        frameErrors++;
        return;
    }
    addZeroBits(fullBits - 1);
    state = MID_FRAME_EXPECTING_1;
}

void RMTCharBuilder::addMidFrameBitsExpecting1(uint16_t duration, uint16_t level) {
    if (level != 1) {
        state = DISCARD_STREAM;
        wrongBitErrors++;
        return;
    }

    uint16_t fullBits = durationToFullBits(duration);
    if (fullBits + bitsAccumulated > dataBitsPerFrame) {
        // We have 1 bits that extend into stop bits. We just need to make sure that the duration of
        // the stop bit portion was long enough...
        uint8_t bitsNeeded = dataBitsPerFrame - bitsAccumulated;
        uint16_t dataBitDuration = bitDuration * bitsNeeded;
        uint16_t stopBitDuration = duration - dataBitDuration;
        if (stopBitDuration < minStopBitDuration) {
            state = DISCARD_STREAM;
            shortStopBits++;
        } else {
            addOneBits(bitsNeeded);
            dataBitsCompleted();
            state = START_OF_FRAME;
        }
    } else if (fullBits + bitsAccumulated == dataBitsPerFrame) {
        // This is the case of ending the data bits with a 1, but not having stop bits...a run on
        // frame.
        state = DISCARD_STREAM;
        runOnFrames++;
    } else {
        state = MID_FRAME_EXPECTING_0;
        addOneBits(fullBits);
    }
}

void RMTCharBuilder::addMidFrameBitsExpecting0(uint16_t duration, uint16_t level) {
    if (level != 0) {
        state = DISCARD_STREAM;
        wrongBitErrors++;
        return;
    }

    uint16_t fullBits = durationToFullBits(duration);
    if (fullBits + bitsAccumulated > dataBitsPerFrame) {
        // We've gotten more data bits than we should have in the frame and we've extended into
        // when the stop bits should be.
        state = DISCARD_STREAM;
        runOnFrames++;
    } else if (fullBits + bitsAccumulated == dataBitsPerFrame) {
        // The last bit(s) of the frame was/were 0 so we should have stop bit(s) next...
        state = WAITING_FOR_STOP_BITS;
        addZeroBits(fullBits);
    } else {
        state = MID_FRAME_EXPECTING_1;
        addZeroBits(fullBits);
    }
}

void RMTCharBuilder::stopBitsReceived(uint16_t duration, uint16_t level) {
    if (level != 1) {
        state = DISCARD_STREAM;
        wrongBitErrors++;
        return;
    }

    if (duration < minStopBitDuration) {
        state = DISCARD_STREAM;
        shortStopBits++;
    } else {
        dataBitsCompleted();
        state = START_OF_FRAME;
    }
}

void RMTCharBuilder::addOneBits(uint8_t count) {
    if (count + bitsAccumulated > dataBitsPerFrame) {
        fatalError("Tried to add too many 1 bits to a frame");
    }

    dataBits = (0xffff << (16 - count)) | (dataBits >> count);
    bitsAccumulated += count;
}

void RMTCharBuilder::addZeroBits(uint8_t count) {
    if (count + bitsAccumulated > dataBitsPerFrame) {
        fatalError("Tried to add too many 0 bits to a frame");
    }

    dataBits = dataBits >> count;
    bitsAccumulated += count;
}

void RMTCharBuilder::dataBitsCompleted() {
    if (!inGoodParity()) {
        state = DISCARD_STREAM;
        parityErrors++;
        return;
    }

    if (parity != PARITY_NONE) {
        dataBits &= 0x7ffff;
    }
    dataBits = dataBits >> (16 - dataBitsPerFrame);

    if (bitsPerChar <= 8) {
        uint8_t uint8DataBits = (uint8_t)dataBits;
        if (xQueueSendToBack(outputQueue, &uint8DataBits, 0) != pdTRUE) {
            overrunErrors++;
        }
    } else {
        if (xQueueSendToBack(outputQueue, &dataBits, 0) != pdTRUE) {
            overrunErrors++;
        }
    }

    // Obviously this gets removed.
//    logger() << logDebugRMTUART << "  Received " << (char)dataBits << Hex << " ("
//             << (uint16_t)dataBits << ")" << eol;
}

bool RMTCharBuilder::inGoodParity() {
    switch (parity) {
        case PARITY_NONE:
            return true;
        case PARITY_EVEN:
            return evenNumberDataBits();
        case PARITY_ODD:
            return !evenNumberDataBits();
        case PARITY_MARK:
            return (dataBits & 0x8000) == 0x8000;
        case PARITY_SPACE:
            return (dataBits & 0x8000) == 0x0000;
        default:
            fatalError("Bad parity configuration foudn in RMT parity check");
    }
}
bool RMTCharBuilder::evenNumberDataBits() {
    uint16_t parity = 0x0000;
    uint16_t scratchDataBits = dataBits;
    for (uint8_t bitsRemaining = dataBitsPerFrame; bitsRemaining; bitsRemaining--) {
        parity ^= scratchDataBits & 0x8000;
        scratchDataBits <<= 1;
    }
    return parity == 0x0000;
}

uint16_t RMTCharBuilder::durationToFullBits(uint16_t duration) {
    return (duration + bitDurationSlop) / bitDuration; 
}
