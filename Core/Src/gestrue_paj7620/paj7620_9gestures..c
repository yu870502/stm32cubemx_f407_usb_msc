
/*
   Copyright (c) 2015 seeed technology inc.
   Website    : www.seeed.cc
   Author     : Wuruibin
   Modified Time: June 2015
   Description: This demo can recognize 9 gestures and output the result, including move up, move down, move left, move right,
  				move forward, move backward, circle-clockwise, circle-counter clockwise, and wave.

   The MIT License (MIT)

   Permission is hereby granted, free of charge, to any person obtaining a copy
   of this software and associated documentation files (the "Software"), to deal
   in the Software without restriction, including without limitation the rights
   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
   copies of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be included in
   all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
   THE SOFTWARE.
*/
#include <stdio.h>
#include <string.h>

#include "cmsis_os.h"

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2c.h"

#include "paj7620.h"

/*
  Notice: When you want to recognize the Forward/Backward gestures, your gestures' reaction time must less than GES_ENTRY_TIME(0.8s).
        You also can adjust the reaction time according to the actual circumstance.
*/
#define GES_REACTION_TIME		500				// You can adjust the reaction time according to the actual circumstance.
#define GES_ENTRY_TIME			800				// When you want to recognize the Forward/Backward gestures, your gestures' reaction time must less than GES_ENTRY_TIME(0.8s). 
#define GES_QUIT_TIME			1000

#define ALL_OFF 0
#define ALL_ON 1
#define LED1_ON 2
#define LED1_OFF 3
#define LED2_ON 4
#define LED2_OFF 5
#define LED1_TWINKLE 6
#define LED2_TWINKLE 7
#define LED_FLOW 8

// 模式
int mode = 0;
extern int8_t gesture_polling(void);

void gesture_scan_task(void const * argument)
{
  for(;;)
  {
    gesture_polling();
    osDelay(500);
  }
}

int8_t gesture_init(void)
{
  int8_t error = 0;

  error = paj7620Init();			// initialize Paj7620 registers
  if (error)
  {
    printf("INIT ERROR,CODE = %d\r\n", error);
    return -1;
  }
  else
  {
    printf("INIT OK\r\n");
  }

  osThreadDef(gesture_scan, gesture_scan_task, osPriorityAboveNormal, 0, 1024);
  osThreadId gesture_scan_task = osThreadCreate(osThread(gesture_scan), NULL); 

  return error;
}

int8_t gesture_polling(void)
{
  uint8_t data = 0, data1 = 0;
  int8_t error = 0;

  error = paj7620ReadReg(0x43, 1, &data);				// Read Bank_0_Reg_0x43/0x44 for gesture result.
  if(error)
  {
    printf("paj7620ReadReg %#X failed, code=%d\r\n", 0x43, error);
    return -1;
  }

    // printf("paj7620ReadReg %#X ok, data=%d\r\n", 0x43, data);
  {
    switch (data) 									// When different gestures be detected, the variable 'data' will be set to different values by paj7620ReadReg(0x43, 1, &data).
    {
      case GES_RIGHT_FLAG:
        delay(GES_ENTRY_TIME);
        paj7620ReadReg(0x43, 1, &data);
        if (data == GES_FORWARD_FLAG)
        {
          printf("Forward\r\n");
          mode = ALL_OFF;
          delay(GES_QUIT_TIME);
        }
        else if (data == GES_BACKWARD_FLAG)
        {
          printf("Backward\r\n");
          mode = ALL_ON;
          delay(GES_QUIT_TIME);
        }
        else
        {
          printf("Right\r\n");
        }
        break;
      case GES_LEFT_FLAG:
        delay(GES_ENTRY_TIME);
        paj7620ReadReg(0x43, 1, &data);
        if (data == GES_FORWARD_FLAG)
        {
          printf("Forward\r\n");
          mode = ALL_OFF;
          delay(GES_QUIT_TIME);
        }
        else if (data == GES_BACKWARD_FLAG)
        {
          printf("Backward\r\n");
          mode = ALL_ON;
          delay(GES_QUIT_TIME);
        }
        else
        {
          printf("Left\r\n");
        }
        break;
      case GES_UP_FLAG:
        delay(GES_ENTRY_TIME);
        paj7620ReadReg(0x43, 1, &data);
        if (data == GES_FORWARD_FLAG)
        {
          printf("Forward\r\n");
          mode = ALL_OFF;
          delay(GES_QUIT_TIME);
        }
        else if (data == GES_BACKWARD_FLAG)
        {
          printf("Backward\r\n");
          mode = ALL_ON;
          delay(GES_QUIT_TIME);
        }
        else
        {
          printf("Up\r\n");
          mode = ALL_ON;
        }
        break;
      case GES_DOWN_FLAG:
        delay(GES_ENTRY_TIME);
        paj7620ReadReg(0x43, 1, &data);
        if (data == GES_FORWARD_FLAG)
        {
          printf("Forward\r\n");
          mode = ALL_OFF;
          delay(GES_QUIT_TIME);
        }
        else if (data == GES_BACKWARD_FLAG)
        {
          printf("Backward\r\n");
          mode = ALL_ON;
          delay(GES_QUIT_TIME);
        }
        else
        {
          printf("Down\r\n");
          mode = ALL_OFF;
        }
        break;
      case GES_FORWARD_FLAG:
        printf("Forward\r\n");
        mode = ALL_OFF;
        delay(GES_QUIT_TIME);
        break;
      case GES_BACKWARD_FLAG:
        printf("Backward\r\n");
        mode = ALL_ON;
        delay(GES_QUIT_TIME);
        break;
      case GES_CLOCKWISE_FLAG:
        printf("Clockwise\r\n");
        mode = LED1_TWINKLE;
        break;
      case GES_COUNT_CLOCKWISE_FLAG:
        printf("anti-clockwise\r\n");
        mode = LED2_TWINKLE;
        break;
      default:
        paj7620ReadReg(0x44, 1, &data1);
        if (data1 == GES_WAVE_FLAG)
        {
          printf("wave\r\n");
          mode = LED_FLOW;
        }
        break;
    }
  }
	return 0;
}

