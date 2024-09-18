
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
#include <stdlib.h>

#include "cmsis_os.h"

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2c.h"

#include "paj7620_9gestures.h"
#include "paj7620.h"

/*
  Notice: When you want to recognize the Forward/Backward gestures, your gestures' reaction time must less than GES_ENTRY_TIME(0.8s).
        You also can adjust the reaction time according to the actual circumstance.
*/
#define GES_REACTION_TIME		500				// You can adjust the reaction time according to the actual circumstance.
#define GES_ENTRY_TIME			800				// When you want to recognize the Forward/Backward gestures, your gestures' reaction time must less than GES_ENTRY_TIME(0.8s). 
#define GES_QUIT_TIME			1000

static gesture_t *__g_gesture_obj = NULL;

void gestureProcessTask(void const * argument)
{
  uint32_t notifyVal = 0;
  uint8_t data = 0;
  int8_t ret = -1;

  for(;;)
  {
    printf("Watting gesture event\r\n");
    notifyVal = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    printf("Receive gesture event = %d\r\n", notifyVal);

    if(0 != (ret = paj7620ReadReg(PAJ7620_ADDR_GES_PS_DET_FLAG_0, 1, &data))){
      printf("paj7620ReadReg %#X failed, code=%d\r\n", PAJ7620_ADDR_GES_PS_DET_FLAG_0, ret);
      return;
    }
    printf("Reg %#X val = %d\r\n", PAJ7620_ADDR_GES_PS_DET_FLAG_0, data);

    switch(data){
      case GES_RIGHT_FLAG:
        printf("Right event\r\n"); break;
      case GES_LEFT_FLAG:
        printf("Left event\r\n"); break;
      case GES_UP_FLAG:
        printf("Up event\r\n"); break;
      case GES_DOWN_FLAG:
        printf("Down event\r\n"); break;
      case GES_FORWARD_FLAG:
        printf("Forwared event\r\n"); break;
      case GES_BACKWARD_FLAG:
        printf("Backward event\r\n"); break;
      case GES_CLOCKWISE_FLAG:
        printf("Clockwise event\r\n"); break;
      case GES_COUNT_CLOCKWISE_FLAG:
        printf("Count clockwise event\r\n"); break;
      default:
        printf("Unknow gesture!!!\r\n");  break;
    }

    if(0 != (ret = paj7620ReadReg(PAJ7620_ADDR_GES_PS_DET_FLAG_1, 1, &data))){
      printf("paj7620ReadReg %#X failed, code=%d\r\n", PAJ7620_ADDR_GES_PS_DET_FLAG_1, ret);
      return;
    }
    printf("Reg %#X val = %d\r\n", PAJ7620_ADDR_GES_PS_DET_FLAG_1, data);
    if(GES_WAVE_FLAG == data)
      printf("Wave event\r\n");
  }
}

int8_t gesture_init(void)
{
  int8_t ret = -1;
  
  if(NULL == (__g_gesture_obj = calloc(1, sizeof(gesture_t)))){
    printf("create gesture obj failed\r\n");
    return -1;
  }

  // initialize Paj7620 registers
  if(0 != (ret = paj7620Init())){
    printf("paj7620Init failed, ret code = %d\r\n", ret);
    ret = -1;
    goto free_gesture_obj;
  }
  else{
    printf("paj7620Init OK\r\n");
    ret = 0;
  }

  osThreadDef(gesture_scan, gestureProcessTask, osPriorityRealtime, 0, 1024);
  __g_gesture_obj->gestureProcessTask = osThreadCreate(osThread(gesture_scan), NULL);
  if(!__g_gesture_obj->gestureProcessTask){
    ret = -1;
    goto free_gesture_obj;
  }

  goto exit;

free_gesture_obj:
  free(__g_gesture_obj);
  __g_gesture_obj = NULL;

exit:
  return ret;
}

gesture_t *getGestureObj(void)
{
  return __g_gesture_obj;
}

int8_t gestureDeinit(void)
{
	return -1;
}


int8_t get_objBright(uint8_t *data)
{
  int8_t ret = -1;

  if((ret = paj7620ReadReg(0xb0, 1, data)) != 0){
    printf("paj7620ReadReg %#X failed, code=%d\r\n", 0xb0, ret);
    ret = -1;
  }
  else{
    // printf("Object bright = %d\r\n", *data);
    ret = 0;
  }

  return ret;
}

int8_t get_objSize(uint16_t *objSize)
{
  uint8_t data = 0;
  int8_t ret = -1;

  if((ret = paj7620ReadReg(0xb2, 1, &data)) != 0){
    printf("paj7620ReadReg %#X failed, code=%d\r\n", 0xb2, ret);
  }
  else{
    *objSize |= (uint16_t)(data & 0x0f) << 8;  //Only 4 bits are needed in the high register
    if((ret = paj7620ReadReg(0xb1, 1, &data)) != 0){
      printf("paj7620ReadReg %#X failed, code=%d\r\n", 0xb1, ret);
    }
    else{
      *objSize |= (uint16_t)data;
      // printf("Object size = %d\r\n", objSize);
      ret = 0;
    }
  }
  return ret;
}

/**
 * ret: -1,get gesture object error, 0,give gesture notify, 1, No interruption occurred
 */
int8_t gestureEXTINotify(void)
{
  gesture_t *gestureObj = getGestureObj();

  if(!gestureObj){
    printf("getGestureObj return NULL\r\n");
    return -1;
  }

  BaseType_t pxHigherPriorityTaskWoken = pdTRUE;
  vTaskNotifyGiveFromISR(gestureObj->gestureProcessTask, &pxHigherPriorityTaskWoken);
  return 0;
}
