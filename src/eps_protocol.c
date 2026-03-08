#include "eps_protocol.h"
#include "kiss_protocol.h"
#include "string.h"

osMutexId_t epsProtocolMutexHandle;
const osMutexAttr_t epsProtocolMutex_attributes = {
  .name = "epsProtocolMutex"
};

void eps_init(){
  epsProtocolMutexHandle = osMutexNew(&epsProtocolMutex_attributes);
}

void eps_get_all_kiss_command(eps_command_t *command){
    if (command == NULL){
        return;
    }
    uint8_t temp[8];
    uint16_t tempLen = 0;
    temp[tempLen++] = EPS_GET_ALL;
    command->len = 0;
    command->len = KISS_Encode(temp,tempLen,command->cmd);
}

void eps_get_vi_sensor_kiss_command(uint8_t channel, eps_command_t *command){
    if (command == NULL){
        return;
    }
    if (channel >= EPS_NUM_VI_CHANNEL){
        channel = EPS_NUM_VI_CHANNEL;
    }
    uint8_t temp[8];
    uint16_t tempLen = 0;
    temp[tempLen++] = EPS_GET_VI_SENSOR;
    temp[tempLen++] = channel;
    command->len = 0;
    command->len = KISS_Encode(temp,tempLen,command->cmd);
}

void eps_get_output_sensor_kiss_command(uint8_t channel, eps_command_t *command){
    if (command == NULL){
        return;
    }
    if (channel >= EPS_NUM_OUTPUT_CHANNEL){
        channel = EPS_NUM_OUTPUT_CHANNEL;
    }
    uint8_t temp[8];
    uint16_t tempLen = 0;
    temp[tempLen++] = EPS_GET_OUTPUT_SENSOR;
    temp[tempLen++] = channel;
    command->len = 0;
    command->len = KISS_Encode(temp,tempLen,command->cmd);
}

void eps_get_output_state_kiss_command(uint8_t channel, eps_command_t *command){
    if (command == NULL){
        return;
    }
    if (channel >= EPS_NUM_OUTPUT_CHANNEL){
        channel = EPS_NUM_OUTPUT_CHANNEL;
    }
    uint8_t temp[8];
    uint16_t tempLen = 0;
    temp[tempLen++] = EPS_GET_SWITCH;
    temp[tempLen++] = channel;
    command->len = 0;
    command->len = KISS_Encode(temp,tempLen,command->cmd);
}

void eps_set_output_state_kiss_command(uint8_t channel, eps_command_t *command){
    if (command == NULL){
        return;
    }
    if (channel >= EPS_NUM_OUTPUT_CHANNEL){
        channel = EPS_NUM_OUTPUT_CHANNEL;
    }
    uint8_t temp[8];
    uint16_t tempLen = 0;
    temp[tempLen++] = EPS_SET_SWITCH;
    temp[tempLen++] = channel;
    command->len = 0;
    command->len = KISS_Encode(temp,tempLen,command->cmd);
}

void eps_get_battery_temperature_kiss_command(uint8_t channel, eps_command_t *command){
    if (command == NULL){
        return;
    }
    if (channel >= EPS_NUM_TEMP_BATT){
        channel = EPS_NUM_TEMP_BATT;
    }
    uint8_t temp[8];
    uint16_t tempLen = 0;
    temp[tempLen++] = EPS_GET_TEMP_BAT;
    temp[tempLen++] = channel;
    command->len = 0;
    command->len = KISS_Encode(temp,tempLen,command->cmd);
}

/* ── Private packing helpers (big-endian) ─────────────────────────────── */

static void pack_u8(uint8_t *buf, uint16_t *offset, uint8_t value)
{
    buf[*offset] = value;
    *offset      = (uint16_t)(*offset + 1U);
}

static void pack_i16_be(uint8_t *buf, uint16_t *offset, int16_t value)
{
    uint16_t u;
    (void)memcpy(&u, &value, sizeof(u));            /* MISRA 11.3: no direct type-pun */
    buf[*offset]        = (uint8_t)((u >> 8U) & 0xFFU);  /* MSB first */
    buf[*offset + 1U]   = (uint8_t)( u        & 0xFFU);  /* LSB second */
    *offset             = (uint16_t)(*offset + 2U);
}

static void pack_data_state(uint8_t *buf, uint16_t *offset, eps_data_state state)
{
    /* Cast enum -> uint8_t; enum values 0-3 are guaranteed to fit */
    pack_u8(buf, offset, (uint8_t)state);
}

static void pack_float_fixed(uint8_t *buf, uint16_t *offset, float value)
{
    float   scaled = value * TEMP_SCALE_FACTOR;
    int16_t fixed;

    /* Clamp to int16_t range before cast — required for defined behaviour */
    if (scaled > TEMP_SCALED_MAX)
    {
        scaled = TEMP_SCALED_MAX;
    }
    else if (scaled < TEMP_SCALED_MIN)
    {
        scaled = TEMP_SCALED_MIN;
    }
    else
    {
        /* All other values are in range — required else branch (MISRA 15.7) */
    }

    fixed = (int16_t)scaled;
    pack_i16_be(buf, offset, fixed);
}

/**
 * @brief  Pack eps_sensor_data_t into a big-endian byte array.
 *
 * Layout per element:
 *   vi_sensor      : [voltage H][voltage L][current H][current L][channel][data_state]
 *   output_sensor  : [voltage H][voltage L][current H][current L][channel][data_state]
 *   output_state   : [status][channel][data_state]
 *   battery_temp   : [temp_x100 H][temp_x100 L][channel][data_state]
 *
 * @param[out] buf      Destination buffer (must be >= EPS_PACKED_BUFFER_SIZE bytes).
 * @param[in]  buf_len  Caller-supplied buffer length guard.
 * @param[in]  data     Source structure (must not be NULL).
 * @return     Bytes written (== EPS_PACKED_BUFFER_SIZE), or 0 on error.
 */
uint16_t eps_pack_sensor_data(uint8_t                  *buf,
                              uint16_t                  buf_len,
                              const eps_sensor_data_t  *data)
{
    uint16_t offset = 0U;
    uint8_t  i;

    if ((buf == NULL) || (data == NULL) ||
        (buf_len < (uint16_t)EPS_PACKED_BUFFER_SIZE))
    {
        return 0U;
    }

    /* VI sensors -------------------------------------------------------- */
    for (i = 0U; i < (uint8_t)EPS_NUM_VI_CHANNEL; i++)
    {
        pack_i16_be     (buf, &offset, data->vi_sensor[i].voltage);
        pack_i16_be     (buf, &offset, data->vi_sensor[i].current);
        pack_u8         (buf, &offset, data->vi_sensor[i].channel);
        pack_data_state (buf, &offset, data->vi_sensor[i].data_state);
    }

    /* Output sensors ---------------------------------------------------- */
    for (i = 0U; i < (uint8_t)EPS_NUM_OUTPUT_CHANNEL; i++)
    {
        pack_i16_be     (buf, &offset, data->output_sensor[i].voltage);
        pack_i16_be     (buf, &offset, data->output_sensor[i].current);
        pack_u8         (buf, &offset, data->output_sensor[i].channel);
        pack_data_state (buf, &offset, data->output_sensor[i].data_state);
    }

    /* Output states ----------------------------------------------------- */
    for (i = 0U; i < (uint8_t)EPS_NUM_OUTPUT_CHANNEL; i++)
    {
        pack_u8         (buf, &offset, data->output_state[i].status);
        pack_u8         (buf, &offset, data->output_state[i].channel);
        pack_data_state (buf, &offset, data->output_state[i].data_state);
    }

    /* Battery temperatures ---------------------------------------------- */
    for (i = 0U; i < (uint8_t)EPS_NUM_TEMP_BATT; i++)
    {
        pack_float_fixed(buf, &offset, data->battery_temperature[i].temperature);
        pack_u8         (buf, &offset, data->battery_temperature[i].channel);
        pack_data_state (buf, &offset, data->battery_temperature[i].data_state);
    }

    return offset;  /* == EPS_PACKED_BUFFER_SIZE */
}