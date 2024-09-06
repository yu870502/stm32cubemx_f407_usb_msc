#include "main.h"
#include "encoder.h"

#include <stdio.h>

/**
 * 
 */
int create_encoder(void)
{
	int ret = 0;
	return ret;
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
	}
	else{	//A下降沿中断触发时，B高电平，编码器发生正转
		_g_encoder_cnt++;
		printf("_g_encoder_cnt = %d\r\n", _g_encoder_cnt);
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
int8_t EncoderKeyNotifyProcess(void)
{
	printf("Encoder key notify\r\n");

	return ENCODER_TRUE;
}
