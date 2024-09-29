#include "main.h"

#include <stdio.h>
#include <stdlib.h>

#include "encoder.h"
#include "simple_log.h"
#include "simple_menu.h"
#include "stm32f4xx_hal_tim.h"

static encoder_object_t *__g_encoderObj = NULL;

int create_encoder(void)
{
	int ret = 0;

	if(!__g_encoderObj){
		if(NULL == (__g_encoderObj = calloc(1, sizeof(encoder_object_t)))){
			printf("Create encoder object failed\r\n");
			return -1;
		}
	}

	__g_encoderObj->user_process = EncoderKeyNotifyProcess;

	return ret;
}

encoder_object_t *getEncoderObj(void)
{
	if(!__g_encoderObj){
		printf("Encoder object not created\r\n");
		return NULL;
	}
	return __g_encoderObj;
}

int _g_encoder_cnt = 0;

int8_t IsEncoderKeyExti(uint16_t GPIO_Pin)
{
	if(GPIO_Pin != Encoder_Key_Pin)
		return ENCODER_FALSE;
	if(GPIO_PIN_RESET != HAL_GPIO_ReadPin(Encoder_Key_GPIO_Port, Encoder_Key_Pin))
		return ENCODER_FALSE;

	printf("encoder key down interrupt\r\n");

	return ENCODER_TRUE;
}
int8_t EncoderKeyNotifyProcess(void *args)
{
	printf("Encoder key notify\r\n");
	BaseType_t pxHigherPriorityTaskWoken = pdTRUE;
	if(pdPASS != xEventGroupSetBitsFromISR(xMenuEventGrp, MENU_EVENT_CLICK, &pxHigherPriorityTaskWoken)){
		printf("send MENU_EVENT_CLICK failed\r\n");
	}	
	return ENCODER_TRUE;
}

extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim6;
void encoderProcessTask(void const * argument)
{
	uint32_t ecValue = 0;
	uint32_t direction = 0;
	uint32_t tv = 0;

	for(;;)
	{
		tv = __HAL_TIM_GET_COUNTER(&htim3);
		if(ecValue != tv){
			ecValue = tv;
			direction = __HAL_TIM_IS_TIM_COUNTING_DOWN(&htim3);
			LOG_IN("encode value=%d, direction=%d", ecValue, direction);
			if(!direction){	//A下降沿中断触发时，B高电平，编码器发生正转
				_g_encoder_cnt++;
				LOG_IN("_g_encoder_cnt = %d", _g_encoder_cnt);
				BaseType_t pxHigherPriorityTaskWoken = pdTRUE;
				if(pdPASS != xEventGroupSetBitsFromISR(xMenuEventGrp, MENU_EVENT_CLOCKWISE, &pxHigherPriorityTaskWoken)){
					LOG_EOR("send MENU_EVENT_CLOCKWISE failed");
				}
			}
			else{
				_g_encoder_cnt--;
				LOG_IN("_g_encoder_cnt = %d", _g_encoder_cnt);
				BaseType_t pxHigherPriorityTaskWoken = pdTRUE;
				if(pdPASS != xEventGroupSetBitsFromISR(xMenuEventGrp, MENU_EVENT_ANTICCLOCKWISE, &pxHigherPriorityTaskWoken)){
					LOG_EOR("send MENU_EVENT_ANTICCLOCKWISE failed");
				}
			}
		}		
		osDelay(50);
	}
}

int8_t init_encoder(void)
{
	HAL_TIM_Encoder_Start(&htim3,TIM_CHANNEL_ALL);
    HAL_TIM_Base_Start_IT(&htim6);	/* 使能定时器x和定时器更新中断 */

	osThreadDef(encoderTask, encoderProcessTask, osPriorityNormal, 0, 128);
	osThreadId encoderTaskHandle = osThreadCreate(osThread(encoderTask), NULL);

	return 0;
}
