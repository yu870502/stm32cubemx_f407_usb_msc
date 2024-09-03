#include "main.h"
#include "encoder.h"

#include <stdio.h>


int _g_encoder_cnt = 0;

/**
  * @brief  EXTI line detection callbacks.
  * @param  GPIO_Pin Specifies the pins connected EXTI line
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  /* Prevent unused argument(s) compilation warning */
  UNUSED(GPIO_Pin);

  if(Encoder_A_Pin == GPIO_Pin)
  {
    GPIO_PinState pinState = HAL_GPIO_ReadPin(Encoder_B_GPIO_Port, Encoder_B_Pin);
    if(GPIO_PIN_RESET == pinState)  //A下降沿中断时，B低电平，发生反转
    {
      _g_encoder_cnt--;
      printf("_g_encoder_cnt = %d\r\n", _g_encoder_cnt);
    }
    else  //A下降沿中断时，B高电平，发生正转
    {
      _g_encoder_cnt++;
      printf("_g_encoder_cnt = %d\r\n", _g_encoder_cnt);
    }
    //发送信号或者事件给，消费任务
  }

  if(Encoder_Key_Pin == GPIO_Pin)
  {
    GPIO_PinState pinState = HAL_GPIO_ReadPin(Encoder_Key_GPIO_Port, Encoder_Key_Pin);
    if(GPIO_PIN_RESET == pinState)  //key 按下
    {
      printf("Encoder key down\r\n");
    }
    else
    {
      printf("Encoder key up\r\n");
    }
  }  
}

/**
 * 创建编码器对象
 */
int create_encoder(void)
{
	int ret = 0;
	return ret;
}

