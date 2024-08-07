/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fatfs.h"
#include "usart.h"
#include "usb_host.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdarg.h>
#include <string.h>
#include "flexible_button.h"
 
#define RXBUFFERSIZE  256     //最大接收字节数
uint8_t RxBuffer[RXBUFFERSIZE];   //接收数据
uint8_t RxData = 0;
uint8_t Uart1_Rx_Cnt = 0;		//接收缓冲计数
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_USB_HOST_Process(void);

/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void test_app(void)
{
  int a = 7;
  int b = 0;
  int c = 0;
  c = a++;
  b = a;
  printf("a = %d, b = %d, c = %d\r\n", a, b, c);
}

#define ENUM_TO_STR(e) (#e)

typedef enum
{
    USER_BUTTON_0 = 0,
    USER_BUTTON_MAX
} user_button_t;

static char *enum_event_string[] = {
    ENUM_TO_STR(FLEX_BTN_PRESS_DOWN),
    ENUM_TO_STR(FLEX_BTN_PRESS_CLICK),
    ENUM_TO_STR(FLEX_BTN_PRESS_DOUBLE_CLICK),
    ENUM_TO_STR(FLEX_BTN_PRESS_REPEAT_CLICK),
    ENUM_TO_STR(FLEX_BTN_PRESS_SHORT_START),
    ENUM_TO_STR(FLEX_BTN_PRESS_SHORT_UP),
    ENUM_TO_STR(FLEX_BTN_PRESS_LONG_START),
    ENUM_TO_STR(FLEX_BTN_PRESS_LONG_UP),
    ENUM_TO_STR(FLEX_BTN_PRESS_LONG_HOLD),
    ENUM_TO_STR(FLEX_BTN_PRESS_LONG_HOLD_UP),
    ENUM_TO_STR(FLEX_BTN_PRESS_MAX),
    ENUM_TO_STR(FLEX_BTN_PRESS_NONE),
};

static char *enum_btn_id_string[] = {
    ENUM_TO_STR(USER_BUTTON_0),
    ENUM_TO_STR(USER_BUTTON_MAX),
};

static flex_button_t keyGroup[USER_BUTTON_MAX];

static void vKeyEvtCb(void *arg)
{
    flex_button_t *btn = (flex_button_t *)arg;
    printf("id: [%d - %s]  event: [%d - %30s]  repeat: %d\r\n",
           btn->id, enum_btn_id_string[btn->id],
           btn->event, enum_event_string[btn->event],
           btn->click_cnt);
    switch (btn->id)
    {
    case USER_BUTTON_0:
    {
        switch (btn->event)
        {
        case FLEX_BTN_PRESS_CLICK:
        {
            printf("key click!\r\n");
        }
        break;
        case FLEX_BTN_PRESS_DOUBLE_CLICK:
        {
            printf("double click!\r\n");
        }
        break;
        case FLEX_BTN_PRESS_LONG_HOLD:
        {
            printf("key long hold!\r\n");
        }
        break;

        default:
            break;
        }
    }
    break;
    }
}


static uint8_t u8KeyRead(void *arg)
{
  return (uint8_t)HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_6);
}

void register_key(void)
{
	int i = 0;
  memset(keyGroup, 0, sizeof(keyGroup));
  for (i = 0; i < USER_BUTTON_MAX; i++)
  {
      keyGroup[i].id = i;
      keyGroup[i].usr_button_read = u8KeyRead;
      keyGroup[i].cb = vKeyEvtCb;
      keyGroup[i].pressed_logic_level = 0;
      keyGroup[i].debounce_tick = 20;
      keyGroup[i].max_multiple_clicks_interval = FLEX_MS_TO_SCAN_CNT(500); // 单击间隔500ms
      keyGroup[i].short_press_start_tick = FLEX_MS_TO_SCAN_CNT(1500);
      keyGroup[i].long_press_start_tick = FLEX_MS_TO_SCAN_CNT(3000);
      keyGroup[i].long_hold_start_tick = FLEX_MS_TO_SCAN_CNT(5000);
      flex_button_register(&keyGroup[i]);
  }  
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_FATFS_Init();
  MX_USB_HOST_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  HAL_UART_Receive_IT(&huart1, (uint8_t *)&RxData, 1);
  printf("Build data:%s, time:%s\r\n", __DATE__, __TIME__);
  register_key();
  // test_app();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
    MX_USB_HOST_Process();

    /* USER CODE BEGIN 3 */
    flex_button_scan();
    HAL_Delay(1);
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

// 定义 def_printf 函数
void def_printf(const char *format, ...)
{
  va_list args;
  char pbuff[256] = {0};

  // 初�?��? args 以获�?? format 之后的参�??
  va_start(args, format);

  // // 调用 vprintf，将格式化字符串和参数列表输出到标准输出
  // vprintf(format, args);

  vsprintf(pbuff, format, args);
  printf("%s", pbuff);

  // 清理 args
  va_end(args);

  while(HAL_UART_GetState(&huart1) != HAL_UART_STATE_READY);//�??测UART发�?�结�??
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(huart);
  /* NOTE: This function Should not be modified, when the callback is needed,
           the HAL_UART_TxCpltCallback could be implemented in the user file
   */

  // def_printf("%c", RxData);
  HAL_UART_Transmit(&huart1, &RxData, 1,0xFFFF); //将收到的信息发�?�出�??
  while(HAL_UART_GetState(&huart1) != HAL_UART_STATE_READY);//�??测UART发�?�结�??
	
  HAL_UART_Receive_IT(&huart1, (uint8_t *)&RxData, 1);
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
