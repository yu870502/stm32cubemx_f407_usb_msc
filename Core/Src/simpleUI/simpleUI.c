
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cmsis_os.h"
#include "simpleUI.h"
#include "COG.h"

menu_obj_t *currentMenu = NULL;

//----------------------------------main menu---------------------------------------
char *rootMenuItemTittleGrp[] = {
    MAIN_MENU_ITEM_MCU_TEMP,
    MAIN_MENU_ITEM_SET_SCREEN_EV,
    MAIN_MENU_ITEM_SCR_BL,
    MAIN_MENU_ITEM_LOG,
    MAIN_MENU_ITEM_GONGDE,
    MAIN_MENU_ITEM_FW_VERSION,
    NULL,
};
menu_item_obj_t *rootMenuItemList = NULL;
menu_obj_t *rootMenuObj = NULL;
uint8_t rootMenuEventProcess(uint8_t *event)
{
    uint8_t itemTotal = 0;
    switch(*event){
        case MENU_EVENT_CLOCKWISE:      //菜单下滚动封装成api
        itemTotal = rootMenuObj->selectedItemIndex + 1;
        if(itemTotal < rootMenuObj->itemTotal){
            rootMenuObj->selectedItemIndex++;
            if(rootMenuObj->selectedItemIndex - rootMenuObj->currentPageIndex >= PAGE_MAX_LINE){
                if(rootMenuObj->currentPageIndex < rootMenuObj->itemTotal - 1){
                    rootMenuObj->currentPageIndex++;
                }
            }
        }
        break;

        case MENU_EVENT_ANTICCLOCKWISE: //菜单上滚动封装成api
        if(rootMenuObj->selectedItemIndex > 0){
            rootMenuObj->selectedItemIndex--;
            if(rootMenuObj->selectedItemIndex < rootMenuObj->currentPageIndex){
                rootMenuObj->currentPageIndex--;
            }
        }
        break;

        case MENU_EVENT_CLICK:

        break;

        case MENU_EVENT_DOUBLE_CLICK:

        break;

        default:
            printf("Unknow menu event:%d\r\n", *event);
        break;
    }
    printf("Menu name:%s\r\n", rootMenuObj->menuName);
    printf("selectedItem:%d\r\n", rootMenuObj->selectedItemIndex);
    printf("currentPage:%d\r\n\r\n", rootMenuObj->currentPageIndex);

    refreshUIMenu(rootMenuObj);
    return 0;
}

//----------------------------------sub1 menu---------------------------------------

//----------------------------------sub2 menu---------------------------------------

EventGroupHandle_t xMenuEventGrp;
void menuEventProcessTask(void *arg)
{
    EventBits_t menuEventGrp;
    uint8_t event = MENU_EVENT_NONE;
    for(;;){
        printf("Watting menuEventGrp:%d\r\n", menuEventGrp);
        menuEventGrp = xEventGroupWaitBits(xMenuEventGrp, MENU_EVENT_CLOCKWISE | MENU_EVENT_ANTICCLOCKWISE | MENU_EVENT_CLICK | MENU_EVENT_DOUBLE_CLICK,
                                        pdTRUE, pdFALSE, portMAX_DELAY);
        (menuEventGrp & MENU_EVENT_CLOCKWISE) ? event |= MENU_EVENT_CLOCKWISE : (event = event);
        (menuEventGrp & MENU_EVENT_ANTICCLOCKWISE) ? event |= MENU_EVENT_ANTICCLOCKWISE : (event = event);
        (menuEventGrp & MENU_EVENT_CLICK) ? event |= MENU_EVENT_CLICK : (event = event);
        (menuEventGrp & MENU_EVENT_DOUBLE_CLICK) ? event |= MENU_EVENT_DOUBLE_CLICK : (event = event);
        printf("receive menuEventGrp:%d\r\n", menuEventGrp);
        menuEventGrp = 0;

        if(!currentMenu || !currentMenu->menuEventProcess){
            printf("NULL pointer %p, %p!\r\n", currentMenu, currentMenu->menuEventProcess);
            event = MENU_EVENT_NONE;
            continue;
        }
        currentMenu->menuEventProcess(&event);
        event = MENU_EVENT_NONE;
    }
}

int8_t creatMenuItemList(char *menuItemTittleGrp[])
{
    uint8_t i = 0;
    menu_item_obj_t *newItem = NULL, *preItem = NULL;
    for(i = 0; menuItemTittleGrp[i]; i++){
        if(NULL == (newItem = calloc(1, sizeof(menu_item_obj_t)))){
            printf("Create new item failed\r\n");
            return -1;
        }
        newItem->itemIndex = i;
        newItem->itenTitle = menuItemTittleGrp[i];
        newItem->nextItem = NULL;
        newItem->subMenu = NULL;
        if(!rootMenuItemList){
            preItem = rootMenuItemList = newItem;
            preItem->nextItem = NULL;
        }
        else{
            preItem->nextItem = newItem;
            preItem = newItem;            
        }
    }
    printf("creatMenuItemList success, node numbers:%d\r\n", i);
    return 0;
}

menu_obj_t *createMenuObj(char *menuName, menu_item_obj_t *menuItemList, uint8_t menuState, menuEventProcess_t eventProcess)
{
    menu_obj_t *menuObj = NULL;
    if(!menuItemList){
        printf("[%s] para[menuItemList] is NULL\r\n", __FUNCTION__);
        return NULL; 
    }
    if(NULL == (menuObj = calloc(1, sizeof(menu_obj_t)))){
        printf("Create new menu failed!!!\r\n");
        return NULL;
    }

    if(menuName){
        char *nameBuff = calloc(1, strlen(menuName) + 1);
        if(!nameBuff){
            printf("[%s] create nameBuff failed\r\n", __FUNCTION__);
        }
        else{
            menuObj->menuName = nameBuff;
            strcpy(menuObj->menuName, menuName);
        }
    }
    else
        printf("[%s] menuName is NULL\r\n", __FUNCTION__);

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
        printf("Menu event process is NULL\r\n");
    if(MENU_STATE_ACTIVE == menuState)
        currentMenu = menuObj;
    return menuObj;
}

menu_obj_t *getRootMenuObj(void)
{
    if(!rootMenuObj){
        printf("rootMenuObj is NULL\r\n");
        return NULL;
    }
    return rootMenuObj;
}

int8_t clearUILine(uint8_t line)
{
    if(line >= PAGE_MAX_LINE){
        printf("[%s] para [line] error:%d\r\n", __func__, line);
        return -1;
    }
    if(clearDDRAMLine(line)){
        printf("clearPageDDRAM failed\r\n");
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

int8_t refreshUIMenu(menu_obj_t *menuObj)
{
    if(!menuObj){
        printf("[%s] para[menuObj] is NULL\r\n", __FUNCTION__);
        return -1;
    }
    if(!(menuObj->itemList) || !(menuObj->itemList->itenTitle)){
        printf("[%s] menuObj->itemList error, menuObj->itemList:%p, menuObj->itemList->itenTitle:%p\r\n", __FUNCTION__, menuObj->itemList, menuObj->itemList->itenTitle);
        return -1;
    }

    if(MENU_STATE_SLEEP == menuObj->state){
        printf("Menu[%s] inactive state\r\n", menuObj->menuName);
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

void refreshMainMenuTest(void)
{
    refreshUILine(0, "Scr light", 0);
    refreshUILine(1, "Gesture", 0);
    refreshUILine(2, "LOG:ON", 0);
    // refreshUILine(3, "FW:1.0", 1);
    uint8_t len = sizeof(rootMenuItemTittleGrp);
    char buff[10] = {0};
    sprintf(buff, "len=%d", len);
    refreshUILine(3, buff, 1);
}

int8_t userMenuInit(void)
{
    if(creatMenuItemList(rootMenuItemTittleGrp)){
        printf("Create root menu item list failed\r\n");
        return -1;
    }

    rootMenuObj = createMenuObj("Root menu", rootMenuItemList, MENU_STATE_ACTIVE, rootMenuEventProcess);
    if(!rootMenuObj){
        printf("Create root menu failed\r\n");
        return -1;
    }

    volatile menu_item_obj_t *itemList = rootMenuObj->itemList;
    uint8_t fs = 0, line = 0, i = 0;
    for(i = 0; itemList; i++){
        if(rootMenuObj->selectedItemIndex == rootMenuObj->currentPageIndex + i)
            fs = 1;
        else
            fs = 0;
        if(rootMenuObj->currentPageIndex >= PAGE_MAX_LINE)
            line = rootMenuObj->currentPageIndex >> 2;  //X >> 2 = X * 2
        else
            line = rootMenuObj->currentPageIndex;
        refreshUILine(line + i, itemList->itenTitle, fs);
				itemList = itemList->nextItem;
    }

    if(NULL == (xMenuEventGrp = xEventGroupCreate())){
        printf("Create menu event grp failed\r\n");
        return -1;
    }

    osThreadDef(menuEventProcess, menuEventProcessTask, osPriorityNormal, 0, 128);
    osThreadId menuEventProcessTaskHandle = osThreadCreate(osThread(menuEventProcess), NULL);
    if(!menuEventProcessTaskHandle){
        printf("Create menuEventProcessTask failed\r\n");
        return -1;
    }

    //TODO:After all object creation failures, a cleanup action must be performed

    return 0;
}
