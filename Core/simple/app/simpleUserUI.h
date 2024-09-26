#ifndef __SIMPLE_USER_UI_H__
#define __SIMPLE_USER_UI_H__

#include "main.h"

#define ITEM_NAME_MCU_TEMP "TEMP"
#define ITEM_NAME_SCR_REFRESH_RATE "SCR REFRESH"
#define ITEM_NAME_SCR_EV "SCR EV"
#define ITEM_NAME_SCR_BL "SCR BL"
#define ITEM_NAME_LOG "LOG"
#define ITEM_NAME_GONGDE "GongDe"
#define ITEM_NAME_FW_VER "FW VER"

#define ITEM_TITLE_VAR_MCU_TEMP             ITEM_NAME_MCU_TEMP":%.2f"
#define ITEM_TITEL_VAR_SCR_REFRESH_RATE     ITEM_NAME_SCR_REFRESH_RATE":%d"
#define ITEM_TITEL_VAR_SCR_EV               ITEM_NAME_SCR_EV":%d"
#define ITEM_TITLE_VAR_SCR_BL               ITEM_NAME_SCR_BL":%s"
#define ITEM_TITLE_VAR_LOG                  ITEM_NAME_LOG":%s"
#define ITEM_TITLE_GONGDE                   ITEM_NAME_GONGDE":+1"
#define ITEM_TITLE_VAR_FW_VER               ITEM_NAME_FW_VER":%s"

#define SCR_REFRESH_RATE_MAX 5
int8_t userMenuInit(void);

#endif
