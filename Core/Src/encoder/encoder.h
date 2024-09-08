#ifndef __ENCODER__H__
#define __ENCODER__H__

#include "stm32f4xx_hal_gpio.h"
#include "main.h"

#define Encoder_A_Pin GPIO_PIN_10
#define Encoder_A_GPIO_Port GPIOE
#define Encoder_A_EXTI_IRQn EXTI15_10_IRQn
#define Encoder_B_Pin GPIO_PIN_12
#define Encoder_B_GPIO_Port GPIOE
#define Encoder_Key_Pin GPIO_PIN_14
#define Encoder_Key_GPIO_Port GPIOE
#define Encoder_Key_EXTI_IRQn EXTI15_10_IRQn

#define ENCODER_FALSE 0
#define ENCODER_TRUE 1

typedef int8_t (*register_user_process_t)(void *);
typedef int8_t (*user_process_t)(void *);

typedef struct __encoder_object_st
{
    int encoder_cnt;
    
    register_user_process_t register_user_process;
    user_process_t user_process;

}encoder_object_t;

int8_t IsEncoderA_Exti(uint16_t GPIO_Pin);
int8_t EncoderB_Process(void);
int8_t IsEncoderKeyExti(uint16_t GPIO_Pin);
int8_t EncoderKeyNotifyProcess(void *args);

#endif
