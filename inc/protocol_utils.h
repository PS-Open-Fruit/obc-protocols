/*
 * driver-hal.h
 *
 *  Created on: March 6, 2026
 *      Author: bipoe
 */

#ifndef PROTOCOL_UTILS_H_
#define PROTOCOL_UTILS_H_
#include "stdint.h"


// extern CRC_HandleTypeDef hcrc;

#define KISS_CMD_REQUEST_FRAME          0x00
#define KISS_CMD_RESPONSE_FRAME         0x01

uint32_t KISS_UpdateCRC32(uint32_t crc, const uint8_t *data, uint16_t length);
uint32_t KISS_CalculateCRC32(const uint8_t *data, uint16_t length);

#endif