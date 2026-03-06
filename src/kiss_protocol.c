#include "kiss_protocol.h"
#include "hal_stm32.h"

static inline uint16_t escape_and_write(uint8_t* out, uint16_t idx, uint8_t val) {
    if (val == FEND) { 
        out[idx++] = FESC; out[idx++] = TFEND; 
    } else if (val == FESC) { 
        out[idx++] = FESC; out[idx++] = TFESC; 
    } else { 
        out[idx++] = val; 
    }
    return idx;
}

/**
 * @brief  Fast accumulator check - only determines if a complete KISS frame
 *         has been received. Does NOT validate CRC or unescape.
 *         O(1) - just checks the last byte.
 *
 * @param  buf     Accumulated buffer
 * @param  len     Current accumulated length
 * @return true if frame appears complete (starts and ends with FEND), false otherwise
 */
 uint8_t KISS_IsFrameComplete(const uint8_t *buf, uint16_t len)
{
    /* Need at minimum: FEND + CMD + ID + PID + LEN(2) + CRC(4) + FEND = 11 bytes */
    if (len < 11U) return false;

    /* Frame started correctly and the last received byte closes it */
    return (buf[0] == FEND) && (buf[len - 1U] == FEND);
}