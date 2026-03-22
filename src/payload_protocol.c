#include "payload_protocol.h"

uint16_t payload_encode(uint8_t payload_id, uint8_t pid, 
                      uint16_t input_len, const uint8_t *input_buffer, 
                     uint8_t *output_buffer, uint16_t max_output_len) {
    
    if (5 + input_len + 4 > max_output_len) {
        return 0;
    }

    uint16_t index = 0;
    output_buffer[index++] = payload_id;
    output_buffer[index++] = pid;
    
    output_buffer[index++] = (uint8_t)(input_len >> 8);
    output_buffer[index++] = (uint8_t)(input_len & 0xFF);

    if (input_buffer != NULL && input_len > 0) {
        memcpy(&output_buffer[index], input_buffer, input_len);
        index += input_len;
    }

    uint32_t crc = KISS_CalculateCRC32(output_buffer, index);
    
    output_buffer[index++] = (uint8_t)(crc >> 24);
    output_buffer[index++] = (uint8_t)(crc >> 16);
    output_buffer[index++] = (uint8_t)(crc >> 8);
    output_buffer[index++] = (uint8_t)(crc & 0xFF);

    return index;
}

payload_status_t payload_decode(const uint8_t *input_buf, uint16_t input_len,payload_header_t *header,uint8_t *output_payload){
    /* seq (1) + payload_id (1) + pid (1) + dataLen (2) + crc (4)*/
    if (input_len < BASIC_PAYLOAD_FRAME_LEN){
      return PAYLOAD_ERR_FRAMING;
    }
    uint32_t crc = KISS_CalculateCRC32(input_buf,input_len - 4U);
    uint32_t crc_in = (((uint32_t)input_buf[input_len - 4] & 0xFF) << 24) |
                      (((uint32_t)input_buf[input_len - 3] & 0xFF) << 16) |
                      (((uint32_t)input_buf[input_len - 2] & 0xFF) << 8)  |
                        (uint32_t)input_buf[input_len - 1] & 0xFF;

    if (crc != crc_in){
      return PAYLOAD_ERR_CRC;
    }
    header->payload_id = input_buf[0];
    header->pid = input_buf[1];
    header->data_len = (input_buf[2] << 8) | (input_buf[3]);
    if (output_payload != NULL && header->data_len >= 0){
      /* -4 for crc -5 for headers*/
      memcpy(output_payload,&input_buf[4],header->data_len);
    }
    return PAYLOAD_VALID_DATA;
}