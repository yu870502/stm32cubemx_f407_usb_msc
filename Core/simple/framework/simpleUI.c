
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmsis_os.h"
#include "simpleUI.h"
#include "simple_log.h"
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

int8_t clearScr(void)
{
    Lcd12864_ClearScreen();
	return 0;
}

int8_t refreshUILine(uint8_t line, char *cont, uint8_t fs)
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
    if(refreshDDRAMLine(line, cont, fs)){
        LOG_EOR("refreshDDRAMLine failed");
        return -1;
    }

    return 0;
}

int8_t refreshUIMenu(menu_obj_t *menuObj)
{
    if(!menuObj){
        LOG_EOR("[%s] para[menuObj] is NULL", __FUNCTION__);
        return -1;
    }
    if(!(menuObj->itemList) || !(menuObj->itemList->itenTitle)){
        LOG_EOR("[%s] menuObj->itemList error, menuObj->itemList:%p, menuObj->itemList->itenTitle:%p", __FUNCTION__, menuObj->itemList, menuObj->itemList->itenTitle);
        return -1;
    }

    if(MENU_STATE_SLEEP == menuObj->state){
        LOG_EOR("Menu[%s] inactive state", menuObj->menuName);
        return -2;
    }
    menu_item_obj_t *item = menuObj->itemList;
    while(item){
        if(item->itemIndex == menuObj->currentPageIndex){
            break;
        }
        item = item->nextItem;
    }

    clearScr();
    uint8_t i, fs;
    for(i = 0; item && i < PAGE_MAX_LINE; i++){
        (item->itemIndex == menuObj->selectedItemIndex) ? fs = 1 : (fs = 0);
        refreshUILine(i, item->itenTitle, fs);
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

    osThreadDef(menuEventProcess, menuEventProcessTask, osPriorityNormal, 0, 128);
    menuEventProcessTaskHandle = osThreadCreate(osThread(menuEventProcess), NULL);
    if(!menuEventProcessTaskHandle){
        LOG_EOR("Create menuEventProcessTask failed");
        return -1;
    }
    return 0;
}

int8_t creatMenuItemList(menu_item_obj_t **menuItemList, char *menuItemTittleGrp[])
{
    if(!menuItemList){
        LOG_EOR("menuItemList is NUL");
        return -1;
    }
    uint8_t i = 0;
    menu_item_obj_t *newItem = NULL, *preItem = NULL;
    for(i = 0; menuItemTittleGrp[i]; i++){
        if(NULL == (newItem = calloc(1, sizeof(menu_item_obj_t)))){
            LOG_EOR("Create new item failed");
            return -1;
        }
        newItem->itemIndex = i;
        newItem->itenTitle = menuItemTittleGrp[i];
        newItem->nextItem = NULL;
        newItem->subMenu = NULL;
        if(!(*menuItemList)){  //head node
            preItem = *menuItemList = newItem;
            preItem->nextItem = NULL;
        }
        else{
            preItem->nextItem = newItem;
            preItem = newItem;
        }
    }
    LOG_IN("creatMenuItemList success, node numbers:%d", i);
    return 0;
}

menu_obj_t *createMenuObj(char *menuName, menu_item_obj_t *menuItemList, uint8_t menuState, menuEventProcess_t eventProcess)
{
    menu_obj_t *menuObj = NULL;
    if(!menuItemList){
        LOG_EOR("[%s] para[menuItemList] is NULL", __FUNCTION__);
        return NULL; 
    }
    if(NULL == (menuObj = calloc(1, sizeof(menu_obj_t)))){
        LOG_EOR("Create new menu failed!!!");
        return NULL;
    }

    if(menuName){
        char *nameBuff = calloc(1, strlen(menuName) + 1);
        if(!nameBuff){
            LOG_EOR("[%s] create nameBuff failed", __FUNCTION__);
        }
        else{
            menuObj->menuName = nameBuff;
            strcpy(menuObj->menuName, menuName);
        }
    }
    else
        LOG_EOR("[%s] menuName is NULL", __FUNCTION__);

    menuObj->itemList = menuItemList;

    uint8_t i = 0;
    menu_item_obj_t *itemList = menuItemList;
    for(i = 0; itemList; i++){
        itemList = itemList->nextItem;
    }
    menuObj->itemTotal = i;

    menuObj->state = menuState;
    menuObj->currentPageIndex = 0;
    menuObj->selectedItemIndex = 0;
    menuObj->menuEventProcess = eventProcess;
    if(!eventProcess)
        LOG_EOR("Menu event process is NULL");
    if(MENU_STATE_ACTIVE == menuState)
        currentMenu = menuObj;
    return menuObj;
}

