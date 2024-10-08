#ifndef __MCU_OBJ_H__
#define __MCU_OBJ_H__

#include "main.h"
#include "adc.h"

#define DEBUG_PORT huart1
#define MCU_TEMP_ADC_CHANEL hadc1
#define MCU_BAT_ADC_CHANEL hadc1

typedef int8_t( *read_mcu_temp_t)(float *t);
typedef int8_t( *read_mcu_vol_t)(float *v);

typedef struct __MCU_OBJ_ST
{
    float temp;
    read_mcu_temp_t getMcuTemp;

    float vol;
    read_mcu_vol_t getMcuVol;
}mcu_obj_t;

int8_t create_mcu_obj(mcu_obj_t **mcuObj);

int8_t destroy_mcu_obj(mcu_obj_t *mcuObj);

int8_t getMcuTemp(float *t);
int8_t getMcuVol(float *v);

int8_t mcu_obj_start(void);

mcu_obj_t *getMcuObj(void);

#endif
