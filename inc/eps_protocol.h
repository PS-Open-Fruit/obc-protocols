/*
 * eps_helper.h
 *
 *  Created on: Jan 24, 2026
 *      Author: bipoe
 */

#ifndef EPS_PROTOCOL_H_
#define EPS_PROTOCOL_H_

#include "hal_stm32.h" 

#define EPS_GET_VI_SENSOR       0x01U
#define EPS_GET_OUTPUT_SENSOR   0x02U
#define EPS_GET_SWITCH          0x03U
#define EPS_GET_TEMP_BAT        0x04U
#define EPS_GET_ALL             0x06U
#define EPS_SET_SWITCH          0x05U

#define EPS_WAIT_TIMTOUT 300U
#define EPS_NUM_VI_CHANNEL  8U
#define EPS_NUM_OUTPUT_CHANNEL  6U
#define EPS_NUM_TEMP_BATT  2U

#define EPS_BUF_SIZE 128


/* ── Per-field packed sizes (bytes) ───────────────────────────────────── */
/*
 * eps_vi_sensor_t      : int16 voltage(2) + int16 current(2) + uint8 channel(1) + uint8 data_state(1) = 6
 * eps_output_sensor_t  : int16 voltage(2) + int16 current(2) + uint8 channel(1) + uint8 data_state(1) = 6
 * eps_output_state_t   : uint8 status(1)  + uint8 channel(1) + uint8 data_state(1)                    = 3
 * eps_battery_temp_t   : int16 temp_x100(2)                  + uint8 channel(1) + uint8 data_state(1) = 4
 */
#define EPS_VI_SENSOR_PACKED_SIZE      6U
#define EPS_OUTPUT_SENSOR_PACKED_SIZE  6U
#define EPS_OUTPUT_STATE_PACKED_SIZE   3U
#define EPS_BATT_TEMP_PACKED_SIZE      4U

#define EPS_PACKED_BUFFER_SIZE \
    ( (EPS_NUM_VI_CHANNEL      * EPS_VI_SENSOR_PACKED_SIZE    ) + \
      (EPS_NUM_OUTPUT_CHANNEL  * EPS_OUTPUT_SENSOR_PACKED_SIZE) + \
      (EPS_NUM_OUTPUT_CHANNEL  * EPS_OUTPUT_STATE_PACKED_SIZE ) + \
      (EPS_NUM_TEMP_BATT       * EPS_BATT_TEMP_PACKED_SIZE    ) )
/* = 48 + 36 + 18 + 8 = 110 bytes */

/* ── Temperature scaling ──────────────────────────────────────────────── */

#define TEMP_SCALE_FACTOR  100.0F   /* float -> int16_t: degrees * 100 */
#define TEMP_SCALED_MAX    32767.0F
#define TEMP_SCALED_MIN   -32768.0F


typedef struct eps_cmd{
    uint8_t cmd[EPS_BUF_SIZE];
    uint8_t len;
} eps_command_t;


typedef enum {
    EPS_DATA_ERROR,
    EPS_DATA_NO_DATA,
    EPS_DATA_CONSUMED,
    EPS_DATA_OK,
} eps_data_state;

extern osMutexId_t epsProtocolMutexHandle;
extern const osMutexAttr_t epsProtocolMutex_attributes;

typedef struct eps_vi_sensor{
  int16_t voltage;
  int16_t current;
  uint8_t channel;
  eps_data_state data_state;
} eps_vi_sensor_t;

typedef struct eps_output_sensor{
  int16_t voltage;
  int16_t current;
  uint8_t channel;
  eps_data_state data_state;
} eps_output_sensor_t;

typedef struct eps_output_state{
  uint8_t status;
  uint8_t channel;
  eps_data_state data_state;
} eps_output_state_t;

typedef struct eps_battery_temperature{
  float temperature;
  uint8_t channel;
  eps_data_state data_state;
} eps_battery_temperature_t;


typedef struct eps_sensor_data{
  eps_vi_sensor_t vi_sensor[EPS_NUM_VI_CHANNEL];
  eps_output_sensor_t output_sensor[EPS_NUM_OUTPUT_CHANNEL];
  eps_output_state_t output_state[EPS_NUM_OUTPUT_CHANNEL];
  eps_battery_temperature_t battery_temperature[EPS_NUM_TEMP_BATT];
} eps_sensor_data_t;

void eps_init();
void eps_get_vi_sensor_kiss_command(uint8_t channel, eps_command_t *command);
void eps_get_output_sensor_kiss_command(uint8_t channel, eps_command_t *command);
void eps_get_output_state_kiss_command(uint8_t channel, eps_command_t *command);
void eps_set_output_state_kiss_command(uint8_t channel, eps_command_t *command);
void eps_get_battery_temperature_kiss_command(uint8_t channel, eps_command_t *command);
void eps_get_all_kiss_command(eps_command_t *command);
uint16_t eps_pack_sensor_data(uint8_t *buf, uint16_t buf_len, const eps_sensor_data_t *data);

#endif