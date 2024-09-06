#ifndef __GESTURE__
#define __GESTURE__

#include <stdio.h>
#include <string.h>

#include "cmsis_os.h"

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2c.h"

#include "paj7620.h"

typedef struct __gesture__st
{
  osThreadId gestureProcessTask;
  uint8_t gestureName;
}gesture_t;

int8_t gesture_init(void);

gesture_t *getGestureObj(void);

int8_t gestureDeinit(void);

int8_t gestureEXTINotify(void);

#endif


