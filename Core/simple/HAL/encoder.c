#include "main.h"

#include <stdio.h>
#include <stdlib.h>

#include "encoder.h"
#include "simpleUI.h"

static encoder_object_t *__g_encoderObj = NULL;
/**
 * 
 */
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

int8_t IsEncoderA_Exti(uint16_t GPIO_Pin)
{
	if(GPIO_Pin != Encoder_A_Pin)
		return ENCODER_FALSE;
	if(GPIO_PIN_RESET != HAL_GPIO_ReadPin(Encoder_A_GPIO_Port, Encoder_A_Pin))
		return ENCODER_FALSE;

	printf("encoder A interrupt\r\n");

	return ENCODER_TRUE;
}

int _g_encoder_cnt = 0;
int8_t EncoderB_Process(void)
{
	GPIO_PinState pinState = HAL_GPIO_ReadPin(Encoder_B_GPIO_Port, Encoder_B_Pin);
	if(GPIO_PIN_RESET == pinState){	//A下降沿中断触发时，B低电平，编码器反转
		_g_encoder_cnt--;
		printf("_g_encoder_cnt = %d\r\n", _g_encoder_cnt);
		BaseType_t pxHigherPriorityTaskWoken = pdTRUE;
		if(pdPASS != xEventGroupSetBitsFromISR(xMenuEventGrp, MENU_EVENT_ANTICCLOCKWISE, &pxHigherPriorityTaskWoken)){
			printf("send MENU_EVENT_ANTICCLOCKWISE failed\r\n");
		}
	}
	else{	//A下降沿中断触发时，B高电平，编码器发生正转
		_g_encoder_cnt++;
		printf("_g_encoder_cnt = %d\r\n", _g_encoder_cnt);
		BaseType_t pxHigherPriorityTaskWoken = pdTRUE;
		if(pdPASS != xEventGroupSetBitsFromISR(xMenuEventGrp, MENU_EVENT_CLOCKWISE, &pxHigherPriorityTaskWoken)){
			printf("send MENU_EVENT_CLOCKWISE failed\r\n");
		}
	}

	return ENCODER_TRUE;
}

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
