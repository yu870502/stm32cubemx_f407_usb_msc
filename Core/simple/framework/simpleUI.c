
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmsis_os.h"
#include "simpleUI.h"
#include "simple_log.h"
#include "simple_obj.h"
#include "st7567.h"

int8_t clearUILine(uint8_t line)
{
    if(line >= PAGE_MAX_LINE){
        LOG_EOR("[%s] para [line] error:%d", __func__, line);
        return -1;
    }
    if(clearDDRAMLine(line)){
        LOG_EOR("clearPageDDRAM failed");
        return -1;
    }
    return 0;
}

int8_t refreshUILine(uint8_t line, char *cont, unsigned int reverse)
{
    if(line >= PAGE_MAX_LINE){
        LOG_EOR("[%s] para [line] error:%d", __func__, line);
        return -1;
    }
	if(!cont){
		LOG_EOR("[%s] param[cont] NULL", __FUNCTION__);
		return -1;
	}
	if(!strlen((char *)cont)){
		LOG_EOR("[%s] param[len] is 0", __FUNCTION__);
		return -1;
	}    
    if(refreshDDRAMLine(line, cont, reverse)){
        LOG_EOR("refreshDDRAMLine failed");
        return -1;
    }

    return 0;
}

int8_t clearScr(void)
{
    Lcd12864_ClearScreen();
	return 0;
}

int8_t refreshItemLine(uint8_t line, item_obj_t *item)
{
    IS_OBJ_NOT_EXIST(item, -1);
    IS_NULL(item->itemTitle, -1);

    if(line >= PAGE_MAX_LINE){
        LOG_EOR("[%s] para [line] error:%d", __func__, line);
        return -1;
    }

    // LOG_WA("title=%s, type=%d, mode=%d", item->itemTitle, item->type, item->mode);
    unsigned int reverseMask = 0;
    if(item->itemIndex == item->parentMenu->selectedItemIndex){
        if(ITEM_TYPE_TITLE_VARIABLE == item->type){
            char *pos = strchr(item->itemTitle, ITEM_TITLE_DELIMITER);
            uint8_t reversTotal = pos - item->itemTitle;
            // LOG_WA("reversTotal=%d", reversTotal);
            uint8_t i = 0;
            reverseMask = 0;
            if(ITEM_MODE_VIEW == item->mode){
                for(i = 0; i < reversTotal; i++){
                    reverseMask |= (0x8000 >> i);
                }
            }
            else{
                uint8_t offset = reversTotal + 1;   //title:var
                reversTotal = ITEM_TITLE_MAX - offset;
                for(i = 0; i < reversTotal; i++){
                    reverseMask |= (0x8000 >> offset + 1 + i);
                }
            }
        }
        else
            reverseMask = 0xffff;
    }
    else{
        reverseMask = 0;
    }

    // LOG_WA("reverseMask=%#X", reverseMask);
    refreshUILine(line, item->itemTitle, reverseMask);
    return 0;
}

int8_t refreshMenuPage(menu_obj_t *menu)
{
    IS_NULL(menu, -1);
    IS_NULL((menu->itemHead), -1);

    if(MENU_STATE_SLEEP == menu->state){
        LOG_EOR("Menu[%s] inactive state", menu->menuName);
        return -2;
    }
    item_obj_t *item = menu->itemHead;
    while(item){
        if(item->itemIndex == menu->currentPageIndex){
            break;
        }
        item = item->nextItem;
    }

    clearScr();
    uint8_t i;
    for(i = 0; item && i < PAGE_MAX_LINE; i++){
        if(item->loadItemVar)
            item->loadItemVar(item);
        refreshItemLine(i, item);
        item = item->nextItem;
    }
    return 0;
}

static menu_obj_t *currentMenu = NULL;
menu_obj_t *getCurrentMenu(void)
{
    return currentMenu;
}

EventGroupHandle_t xMenuEventGrp;
void menuEventProcessTask(void *arg)
{
    EventBits_t menuEventGrp;
    uint8_t event = MENU_EVENT_NONE;
    for(;;){
        LOG_IN("Watting menuEventGrp:%d", menuEventGrp);
        menuEventGrp = xEventGroupWaitBits(xMenuEventGrp, MENU_EVENT_CLOCKWISE | MENU_EVENT_ANTICCLOCKWISE | MENU_EVENT_CLICK | MENU_EVENT_DOUBLE_CLICK,
                                        pdTRUE, pdFALSE, portMAX_DELAY);
        (menuEventGrp & MENU_EVENT_CLOCKWISE) ? event |= MENU_EVENT_CLOCKWISE : (event = event);
        (menuEventGrp & MENU_EVENT_ANTICCLOCKWISE) ? event |= MENU_EVENT_ANTICCLOCKWISE : (event = event);
        (menuEventGrp & MENU_EVENT_CLICK) ? event |= MENU_EVENT_CLICK : (event = event);
        (menuEventGrp & MENU_EVENT_DOUBLE_CLICK) ? event |= MENU_EVENT_DOUBLE_CLICK : (event = event);
        LOG_IN("receive menuEventGrp:%d", menuEventGrp);
        menuEventGrp = 0;

        if(!currentMenu || !currentMenu->menuEventProcess){
            LOG_EOR("NULL pointer %p, %p!", currentMenu, currentMenu->menuEventProcess);
            event = MENU_EVENT_NONE;
            continue;
        }
        currentMenu->menuEventProcess(&event);
        event = MENU_EVENT_NONE;
    }
}

osThreadId menuEventProcessTaskHandle = NULL;
int8_t startMenuEventProcess(void)
{
    menu_obj_t *menu = NULL;
    if(NULL == (menu = getCurrentMenu())){
        LOG_EOR("getCurrentMenu is NULL");
        return -1;
    }
    if(MENU_STATE_SLEEP == menu->state){
        LOG_EOR("Current dev is in an inactive state");
        return -1; 
    }
    if(menuEventProcessTaskHandle){
        LOG_WA("MenuEventProcess already running");
        return 1;
    }

    if(NULL == (xMenuEventGrp = xEventGroupCreate())){
        LOG_EOR("Create menu event grp failed");
        return -1;
    }
    LOG_IN("startMenuEventProcess");
    osThreadDef(menuEventProcess, menuEventProcessTask, osPriorityNormal, 0, 512);
    menuEventProcessTaskHandle = osThreadCreate(osThread(menuEventProcess), NULL);
    if(!menuEventProcessTaskHandle){
        LOG_EOR("Create menuEventProcessTask failed");
        return -1;
    }
    return 0;
}

menu_obj_t *createMenu(char *menuName, menuEventProcess_t menuEventProcess)
{
    menu_obj_t *menu = NULL;
    CREATE_OBJ(menu, menu_obj_t, NULL);

    if(menuName){
        char *nameBuff = calloc(1, strlen(menuName) + 1);
        if(!nameBuff){
            LOG_EOR("[%s] create nameBuff failed", __FUNCTION__);
        }
        else{
            menu->menuName = nameBuff;
            strncpy(menu->menuName, menuName, strlen(menuName));
        }
    }
    else
        LOG_WA("[%s] menuName is NULL", __FUNCTION__);

    menu->state = MENU_STATE_SLEEP;
    menu->itemTotal = 0;
    menu->itemHead = NULL;
    menu->currentPageIndex = 0;
    menu->selectedItemIndex = 0;
    menu->menuEventProcess = menuEventProcess;
    if(!menuEventProcess)
        LOG_WA("Menu event process is NULL");

    return menu;
}

item_obj_t *createItem(itemType_t type, char *title, loadItemVar_t loadItemVar, ItemEventProcess_t ItemEventProcess)
{
    IS_NULL(title, NULL);
    // IS_NULL(loadItemVar, NULL);
    // IS_NULL(ItemEventProcess, NULL);

    item_obj_t *item = NULL;
    CREATE_OBJ(item, item_obj_t, NULL);
    item->type = type;
    item->mode = ITEM_MODE_VIEW;
    strncpy(item->itemTitle, title, strlen(title));
    item->loadItemVar = loadItemVar;
    item->ItemEventProcess = ItemEventProcess;
    item->itemIndex = 0;
    item->nextItem = NULL;
    item->parentMenu = NULL;
    return item;
}

int8_t addItemToMenu(menu_obj_t *menu, item_obj_t *item)
{
    IS_NULL(menu, -1);
    IS_NULL(item, -1);

    if(!menu->itemHead){
        menu->itemHead = item;
        item->itemIndex = 0;
        menu->itemTotal = 1;
        item->parentMenu = menu;
        return 0;
    }

    item_obj_t *it = menu->itemHead;
    uint8_t i = 1;
    while(it->nextItem){
        i++;
        it = it->nextItem;
    }
    it->nextItem = item;
    item->itemIndex = i;
    item->parentMenu = menu;
    menu->itemTotal = i + 1;

    return 0;
}

int8_t startMenu(menu_obj_t *menu)
{
    IS_NULL(menu, -1);

    currentMenu = menu;
    menu->state = MENU_STATE_ACTIVE;

    refreshMenuPage(menu);
    startMenuEventProcess();
    return 0;
}

