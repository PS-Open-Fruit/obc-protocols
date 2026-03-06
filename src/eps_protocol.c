#include "eps_protocol.h"
#include "kiss_protocol.h"

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