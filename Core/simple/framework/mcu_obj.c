#include "cmsis_os.h"
#include "mcu_obj.h"
#include "simple_log.h"
#include "simple_obj.h"

#include <stdlib.h>
#include <string.h>

static mcu_obj_t *mcuObj = NULL;

static int8_t updateMcuTemp(float t)
{
    if(NULL == (mcuObj = getMcuObj())){
        LOG_EOR("Get mcu obj failed");
        return -1; 
    }
    mcuObj->temp = t;
	return 0;
}

// volatile float localTemp = 0.0f;
static int8_t readMcuTemp(float *arg)
{
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
    
    uint32_t adcVal = 0;
    adcVal = HAL_ADC_GetValue(&MCU_TEMP_ADC_CHANEL);  //获取并计算电电压
    // LOG_DB("HAL_ADC_GetValue : %d", adcVal);
    if(HAL_OK !=HAL_ADC_Stop(&MCU_TEMP_ADC_CHANEL)){  //关闭ADC转换
        LOG_EOR("HAL_ADC_Stop failed");
        return -1;
    }

    float vol = adcVal * (3.3 / 4096);
    // LOG_DB("vol = %.3fV",vol);  //打印输出电压

    char buff[40] = {0};
    float localTemp = (vol - 0.76f) / 0.0025f + 25.0f;
    updateMcuTemp(localTemp);
    memset(buff, 0, sizeof(buff));
    sprintf(buff, "temperature = %.3f \u2013\r\n", localTemp);
    // LOG_DB("%s", buff);

    return 0;
}

static int8_t getMcuTemp(float *t)
{
    if(NULL == (mcuObj = getMcuObj())){
        LOG_EOR("Get mcu obj failed");
        return -1; 
    }
    LOG_IN("t = %p", t);
    *t = mcuObj->temp;
    return 0;
}

int8_t getMcuVol(float *v)
{
    return 0;
}

int8_t create_mcu_obj(mcu_obj_t **mcuObj)
{
    IS_OBJ_EXIST((*mcuObj), 1);
    CREATE_OBJ((*mcuObj), mcu_obj_t, -1);
    LOG_DB("(*mcuObj) = %p", (*mcuObj));

    (*mcuObj)->temp = 0;
    (*mcuObj)->getMcuTemp = getMcuTemp;
    (*mcuObj)->vol = 0;
    (*mcuObj)->getMcuVol = getMcuVol;

    return 0;
}

int8_t destroy_mcu_obj(mcu_obj_t *mcuObj){
    IS_OBJ_NOT_EXIST(mcuObj, -1);
    FREE_OBJ(mcuObj, -1);
}

mcu_obj_t *getMcuObj(void){
    IS_NULL(mcuObj, NULL);
    return mcuObj;
}

void getMCUTempTask(void *arg)
{
    for(;;){
        readMcuTemp(NULL);
        HAL_Delay(1000);
    }
}

int8_t mcu_obj_start(void)
{
    if(create_mcu_obj(&mcuObj)){
        LOG_EOR("Create mcu obj failed");
        return -1;
    }
    LOG_DB("mcuObj = %p", mcuObj);

    osThreadDef(getMCUTemp, getMCUTempTask, osPriorityBelowNormal, 0, 1024);
    osThreadId defaultTaskHandle = osThreadCreate(osThread(getMCUTemp), NULL);
    if(!defaultTaskHandle)
    {
        LOG_EOR("Create getMCUTempTask failed");
        return -1;
    }

    return 0;
}

