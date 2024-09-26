#include "simple_scr.h"
#include <string.h>

scrObj_t scrObj;

int8_t init_scr(void)
{
    scrObj.scrEV = 0X06;
    scrObj.scrRefreshRate = 2;
    scrObj.scrMode = SCR_MODE_NORMAL;
    strncpy(scrObj.scrBl, SCR_BL_OFF, strlen(SCR_BL_OFF));
    return 0;
}
