#include "main.h"
#include "cmsis_os.h"

#include "simple_log.h"
#include "mcu_obj.h"

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

#include "simple_scr.h"

int8_t app_init(void)
{
	LOG_IN("Build data:%s, time:%s\r\n", __DATE__, __TIME__);

    mcu_obj_start();

	Init_ST7567();

	init_scr();

	userMenuInit();

	key_init();

	gesture_init();

    return 0;
}
