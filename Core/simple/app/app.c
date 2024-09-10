#include "main.h"
#include "cmsis_os.h"

#include "simple_log.h"
#include "mcu.h"

#include "task.h"

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "st7567.h"
#include "key.h"

#include "paj7620_9gestures.h"
#include "encoder.h"
#include "simpleUserUI.h"

#include "stm32f4xx_hal_def.h"
#include "mcu.h"

void getMCUTempTask(void *arg)
{
    for(;;){
        get_mcu_temp(NULL);
    }
}

int8_t app_init(void)
{
	LOG_IN("Build data:%s, time:%s\r\n", __DATE__, __TIME__);

    osThreadDef(getMCUTemp, getMCUTempTask, osPriorityNormal, 0, 128);
    osThreadId defaultTaskHandle = osThreadCreate(osThread(getMCUTemp), NULL);
    if(!defaultTaskHandle)
    {
        LOG_EOR("Create getMCUTempTask failed");
        return -1;
    }

	Init_ST7567();
		
	userMenuInit();

	key_init();

	gesture_init();

    return 0;
}
