#include "kiss_protocol.h"
#include "hal_stm32.h"

uint16_t KISS_Encode_Custom_Cmd(const uint8_t *in_buf, uint8_t cmd, uint16_t len, uint8_t *out_buffer){
    uint16_t idx = 0;
    out_buffer[idx++] = FEND;
    out_buffer[idx++] = cmd;

    for (uint16_t i = 0; i < len; i++) {
    uint8_t c = in_buf[i];
    if (c == FEND) {
        out_buffer[idx++] = FESC;
        out_buffer[idx++] = TFEND;
    } else if (c == FESC) {
        out_buffer[idx++] = FESC;
        out_buffer[idx++] = TFESC;
    } else {
        out_buffer[idx++] = c;
    }
    }
    out_buffer[idx++] = FEND;
    return idx;
}

uint16_t KISS_Encode(const uint8_t *in_buf, uint16_t len, uint8_t *out_buffer){
    uint16_t idx = 0;
    out_buffer[idx++] = FEND;
    out_buffer[idx++] = TNC_DATA;

    for (uint16_t i = 0; i < len; i++) {
    uint8_t c = in_buf[i];
    if (c == FEND) {
        out_buffer[idx++] = FESC;
        out_buffer[idx++] = TFEND;
    } else if (c == FESC) {
        out_buffer[idx++] = FESC;
        out_buffer[idx++] = TFESC;
    } else {
        out_buffer[idx++] = c;
    }
    }
    out_buffer[idx++] = FEND;
    return idx;
}

uint16_t KISS_Decode(const uint8_t *in_buf, uint16_t in_len, uint8_t *out_buff){
    if (in_len < 3) return 0;
    if (in_buf[0] != FEND || in_buf[in_len-1] != FEND) return 0;
    
    uint16_t out_idx = 0;
    for (uint16_t i = 2; i < in_len - 1; i++) {
        uint8_t c = in_buf[i];
        if (c == FESC) {
            i++;
            if (i >= in_len - 1) return 0; // Error
            if (in_buf[i] == TFEND) out_buff[out_idx++] = FEND;
            else if (in_buf[i] == TFESC) out_buff[out_idx++] = FESC;
        } else {
            out_buff[out_idx++] = c;
        }
    }
    return out_idx;
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
    /* Need at minimum: FEND + CMD + FEND = 3 bytes */
    if (len < 3U) return false;

    /* Frame started correctly and the last received byte closes it */
    return (buf[0] == FEND) && (buf[len - 1U] == FEND);
}