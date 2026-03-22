/*
 * driver-hal.h
 *
 *  Created on: March 6, 2026
 *      Author: bipoe
 */

#ifndef PAYLOAD_PROTOCOL_H_
#define PAYLOAD_PROTOCOL_H_
#include "stdint.h"
#include "protocol_utils.h"
#include "string.h"

#define BASIC_PAYLOAD_FRAME_LEN 8

typedef struct {
    uint8_t payload_id; 
    uint8_t pid; 
    uint16_t data_len;
} payload_header_t;

typedef enum {
    PAYLOAD_VALID_DATA = 0,
    PAYLOAD_VALID_CMD  = 1,
    PAYLOAD_ERR_EMPTY  = -1,
    PAYLOAD_ERR_PORT   = -2,
    PAYLOAD_ERR_TYPE   = -3,
    PAYLOAD_ERR_FRAMING = -4,
    PAYLOAD_ERR_CRC    = -5,  // Added for CRC validation
    PAYLOAD_ERR_DATA = -6,
} payload_status_t;

uint16_t payload_encode(uint8_t payload_id, uint8_t pid, 
                      uint16_t input_len, const uint8_t *input_buffer, 
                     uint8_t *output_buffer, uint16_t max_output_len);

payload_status_t payload_decode(const uint8_t *input_buf, uint16_t input_len,payload_header_t *header,uint8_t *output_payload);
#endif