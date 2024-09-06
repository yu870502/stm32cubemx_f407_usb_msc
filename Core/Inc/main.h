/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define KEY1_Pin GPIO_PIN_6
#define KEY1_GPIO_Port GPIOE
#define SPI_DC_RS_Pin GPIO_PIN_2
#define SPI_DC_RS_GPIO_Port GPIOA
#define SPI_BLK_Pin GPIO_PIN_3
#define SPI_BLK_GPIO_Port GPIOA
#define SPI_RST_Pin GPIO_PIN_4
#define SPI_RST_GPIO_Port GPIOA
#define SPI_CS_Pin GPIO_PIN_6
#define SPI_CS_GPIO_Port GPIOA
#define Encoder_A_Pin GPIO_PIN_10
#define Encoder_A_GPIO_Port GPIOE
#define Encoder_A_EXTI_IRQn EXTI15_10_IRQn
#define Encoder_B_Pin GPIO_PIN_12
#define Encoder_B_GPIO_Port GPIOE
#define Encoder_Key_Pin GPIO_PIN_14
#define Encoder_Key_GPIO_Port GPIOE
#define Encoder_Key_EXTI_IRQn EXTI15_10_IRQn
#define RS485_E_Pin GPIO_PIN_4
#define RS485_E_GPIO_Port GPIOD
#define RS485_D_Pin GPIO_PIN_5
#define RS485_D_GPIO_Port GPIOD
#define RS485_R_Pin GPIO_PIN_6
#define RS485_R_GPIO_Port GPIOD
#define PAJ7620_Int_Pin GPIO_PIN_8
#define PAJ7620_Int_GPIO_Port GPIOB
#define PAJ7620_Int_EXTI_IRQn EXTI9_5_IRQn

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
