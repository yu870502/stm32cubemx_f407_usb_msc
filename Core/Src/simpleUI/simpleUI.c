
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "simpleUI.h"
#include "COG.h"

static menu_obj_t *__g_menuObj = NULL;

char *mainMenuItemGrp[] = {
    MAIN_MENU_ITEM_MCU_TEMP,
    MAIN_MENU_ITEM_SET_SCREEN_EV,
    MAIN_MENU_ITEM_SCR_BL,
    MAIN_MENU_ITEM_LOG,
    MAIN_MENU_ITEM_FW_VERSION,
};

int8_t clearUILine(uint8_t line)
{
    if(line >= MENU_MAX_LINE){
        printf("[%s] para [line] error:%d\r\n", __func__, line);
        return -1;
    }
    if(clearDDRAMLine(line)){
        printf("clearPageDDRAM failed\r\n");
        return -1;
    }
    return 0;
}

int8_t refreshUILine(uint8_t line, char *cont, uint8_t fs)
{
    if(line >= MENU_MAX_LINE){
        printf("[%s] para [line] error:%d\r\n", __func__, line);
        return -1;
    }
	if(!cont){
		printf("[%s] param[cont] NULL\r\n", __FUNCTION__);
		return -1;
	}
	if(!strlen((char *)cont)){
		printf("[%s] param[len] is 0\r\n", __FUNCTION__);
		return -1;
	}    
    if(refreshDDRAMLine(line, cont, fs)){
        printf("refreshDDRAMLine failed\r\n");
        return -1;
    }

    return 0;
}

void refreshMainMenuTest(void)
{
    refreshUILine(0, "Scr light", 0);
    refreshUILine(1, "Gesture", 0);
    refreshUILine(2, "LOG:ON", 0);
    refreshUILine(3, "FW:1.0", 1);
}

menu_obj_t *createMenuObject(void)
{
    if(__g_menuObj){
        printf("Menu obj already exists\r\n");
        return __g_menuObj;
    }
    if(NULL == (__g_menuObj = calloc(1, sizeof(menu_obj_t)))){
        printf("Create menu obj failed\r\n");
        return NULL;
    }

    __g_menuObj->selectedMenuItemIndex = 0;
    return __g_menuObj;
}

menu_obj_t *getMenuObject(void)
{
    if(!__g_menuObj){
        printf("Menu obj is NULL\r\n");
        return NULL;
    }
    return __g_menuObj;
}

int8_t displayMainMenu(void)
{
	return 0;
}
