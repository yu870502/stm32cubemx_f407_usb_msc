#ifndef __ENCODER__H__
#define __ENCODER__H__

#include "stm32f4xx_hal_gpio.h"

#define Encoder_A_Pin GPIO_PIN_10
#define Encoder_A_GPIO_Port GPIOE
#define Encoder_A_EXTI_IRQn EXTI15_10_IRQn
#define Encoder_B_Pin GPIO_PIN_12
#define Encoder_B_GPIO_Port GPIOE
#define Encoder_Key_Pin GPIO_PIN_14
#define Encoder_Key_GPIO_Port GPIOE
#define Encoder_Key_EXTI_IRQn EXTI15_10_IRQn

typedef int (* register_user_process_t)(void *);
typedef void *(* user_process_t)(void *);

int register_user_process(void *arg)
{
	int ret = 0;
	return ret;
}

typedef struct __encoder_object_st
{
    int encoder_cnt;
    
    register_user_process_t register_user_process;
    user_process_t user_process;

}encoder_object_t;

#endif
