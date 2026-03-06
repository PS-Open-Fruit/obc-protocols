#include "protocol_utils.h"
#include "hal_stm32.h"

extern CRC_HandleTypeDef hcrc;


/**
 * @brief  Incrementally computes the CRC32 using STM32 Hardware
 */
uint32_t KISS_UpdateCRC32(uint32_t crc, const uint8_t *data, uint16_t length) {
    // If this is the start of a new CRC calculation (seed is 0xFFFFFFFF),
    // we reset the hardware peripheral's data register.
    if (crc == 0xFFFFFFFF) {
        __HAL_CRC_DR_RESET(&hcrc);
    }
    
    // If there's no data, just return the current hardware state
    if (length == 0) {
        return hcrc.Instance->DR;
    }

    // Accumulate the new data into the existing hardware state.
    // Note: Cast to uint32_t* is required by HAL, but the hardware 
    // must be configured for Byte (8-bit) input to process it correctly.
    return HAL_CRC_Accumulate(&hcrc, (uint32_t *)data, length);
}

/**
 * @brief  Calculates the CRC32 from scratch using STM32 Hardware
 */
uint32_t KISS_CalculateCRC32(const uint8_t *data, uint16_t length) {
    // HAL_CRC_Calculate automatically resets the peripheral before calculating
    uint32_t hw_crc = HAL_CRC_Calculate(&hcrc, (uint32_t *)data, length);
    
    // return hw_crc ^ 0xFFFFFFFF;
    return hw_crc;
}

