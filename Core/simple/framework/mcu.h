#ifndef __MCU_TEMPERATURE_H__
#define __MCU_TEMPERATURE_H__

#include "main.h"
#include "adc.h"

#define DEBUG_PORT huart1
#define MCU_TEMP_ADC_CHANEL hadc1
#define MCU_BAT_ADC_CHANEL hadc1

typedef int8_t( *get_mcu_temp_t)(float *arg);
typedef int8_t( *get_mcu_vol_t)(float *arg);

typedef struct __MCU_OBJ_ST
{
    float temp;
    get_mcu_temp_t get_mcu_temp;

    float vol;
    get_mcu_vol_t get_mcu_vol;

}mcu_obj_t;

int8_t create_mcu_obj(mcu_obj_t *mcuObj);

int8_t destroy_mcu_obj(mcu_obj_t *mcuObj);

int8_t get_mcu_temp(float *arg);
int8_t get_mcu_vol(float *arg);

#endif
