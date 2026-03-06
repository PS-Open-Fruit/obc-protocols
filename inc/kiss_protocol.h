/*
 * driver-hal.h
 *
 *  Created on: Feb 7, 2026
 *      Author: bipoe
 */

#ifndef KISS_PROTOCOL_H_
#define KISS_PROTOCOL_H_
#include "stdint.h"

// --- Legacy Definitions ---
#define FEND    0xC0    /* Frame End - marks start and end of frame */
#define FESC    0xDB    /* Frame Escape - escape character */
#define TFEND   0xDC    /* Transposed Frame End - escaped FEND */
#define TFESC   0xDD    /* Transposed Frame Escape - escaped FESC */
#define TNC_DATA 0x00   /* TNC Data command byte */

typedef enum {
    KISS_VALID_DATA = 0,
    KISS_VALID_CMD  = 1,
    KISS_ERR_EMPTY  = -1,
    KISS_ERR_PORT   = -2,
    KISS_ERR_TYPE   = -3,
    KISS_ERR_FRAMING = -4,
    KISS_ERR_CRC    = -5  // Added for CRC validation
} kiss_status_t;

uint8_t KISS_IsFrameComplete(const uint8_t *buf, uint16_t len);
kiss_status_t KISS_Init();
uint16_t KISS_Encode(const uint8_t *in_buf, uint16_t len, uint8_t *out_buffer);
uint16_t KISS_Decode(const uint8_t *in_buf, uint16_t in_len, uint8_t *out_buff);

#endif