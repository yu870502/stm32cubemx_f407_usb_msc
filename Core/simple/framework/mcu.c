#include "mcu.h"
#include "simple_log.h"
#include "simple_obj.h"

#include <stdlib.h>
#include <string.h>

int8_t get_mcu_temp(float *arg)
{
    uint32_t adcVal = 0;

    if(HAL_OK != HAL_ADC_Start(&MCU_TEMP_ADC_CHANEL)){  //开始ADC转换
        LOG_EOR("HAL_ADC_Start failed");
        return -1;
    }
    if(HAL_OK !=HAL_ADC_PollForConversion(&MCU_TEMP_ADC_CHANEL,10)){  //等待ADC转换结束
        LOG_EOR("HAL_ADC_PollForConversion failed");
        return -1;
    }
    if(HAL_IS_BIT_CLR(HAL_ADC_GetState(&MCU_TEMP_ADC_CHANEL), HAL_ADC_STATE_REG_EOC)){ //获取转换状态
        LOG_EOR("ADC state is clear");
        return -1; 
    }
    adcVal = HAL_ADC_GetValue(&MCU_TEMP_ADC_CHANEL);  //获取并计算电电压
    LOG_IN("HAL_ADC_GetValue : %d", adcVal);
    if(HAL_OK !=HAL_ADC_Stop(&MCU_TEMP_ADC_CHANEL)){  //关闭ADC转换
        LOG_EOR("HAL_ADC_Stop failed");
        return -1;
    }

    float vol = adcVal * (3.3 / 4096);
    LOG_IN("vol = %.3fV",vol);  //打印输出电压

    char buff[40] = {0};
    float C = (vol - 0.76) / 0.0025 + 25.0;
    memset(buff, 0, sizeof(buff));
    sprintf(buff, "C = %.3f \u2013\r\n", C);
    LOG_IN("%s", buff);
    HAL_Delay(2000);

    return 0;
}

int8_t get_mcu_vol(float *arg)
{
    return 0;
}

int8_t create_mcu_obj(mcu_obj_t *mcuObj)
{
    CREATE_OBJ(mcuObj, mcu_obj_t);

    mcuObj->temp = 0;
    mcuObj->get_mcu_temp = get_mcu_temp;
    mcuObj->vol = 0;
    mcuObj->get_mcu_vol = get_mcu_vol;
	
    return 0;
}

int8_t destroy_mcu_obj(mcu_obj_t *mcuObj){
    // IS_OBJ_NOT_EXIST(mcuObj);
    FREE_OBJ(mcuObj);
		return 0;
}



