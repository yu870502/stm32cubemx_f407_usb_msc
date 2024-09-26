#ifndef __SIMPLE_SCR_H_
#define __SIMPLE_SCR_H_

#include "main.h"

#define SCR_MODE_NORMAL 0
#define SCR_MODE_SLEEP 1

#define SCR_BL_ON "ON"
#define SCR_BL_OFF "OFF"

typedef struct __simple_obj_st
{
    uint8_t scrEV;
    uint8_t scrRefreshRate;
    uint8_t scrMode;
    char scrBl[4];
}scrObj_t;

extern scrObj_t scrObj;

int8_t init_scr(void);

#endif
