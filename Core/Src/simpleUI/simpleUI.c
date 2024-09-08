
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "simpleUI.h"
#include "COG.h"

menu_item_obj_t *rootMenuItemList = NULL;

menu_obj_t *rootMenuObj = NULL;

char *rootMenuItemTittleGrp[] = {
    MAIN_MENU_ITEM_MCU_TEMP,
    MAIN_MENU_ITEM_SET_SCREEN_EV,
    MAIN_MENU_ITEM_SCR_BL,
    MAIN_MENU_ITEM_LOG,
    MAIN_MENU_ITEM_GONGDE,
    MAIN_MENU_ITEM_FW_VERSION,
    NULL,
};

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

menu_obj_t *createMenuObj(menu_item_obj_t *menuItemList, uint8_t menuState)
{
    if(!menuItemList){
        printf("[%s] para[menuItemList] is NULL\r\n", __FUNCTION__);
        return rootMenuObj; 
    }
    if(rootMenuObj){
        printf("rootMenuObj already exists\r\n");
        return rootMenuObj;
    }
    if(NULL == (rootMenuObj = calloc(1, sizeof(menu_obj_t)))){
        printf("Create new menu failed!!!\r\n");
        return NULL;
    }
    rootMenuObj->itemList = menuItemList;
    rootMenuObj->state = menuState;
    rootMenuObj->currentPageIndex = 0;
    rootMenuObj->selectedItemIndex = 0;
    return rootMenuObj;
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
/**
 * Display the full menu of the entire page
 * startItemIndex:Index of menu items in the menu group
 */
#if 0
int8_t refreshUIPage(char **startItem, uint8_t startItemIndex, uint8_t reversDisplayIndex)
{
	if(!startItem){
		printf("[%s] param[startItem] NULL\r\n", __FUNCTION__);
		return -1;
	}
	if(startItemIndex >= MENU_INDEX_ITEMS_MAX){
		printf("[%s] param[startItemIndex] error:%d\r\n", __FUNCTION__, startItemIndex);
		return -1;
	}    
	if(reversDisplayIndex >= MENU_INDEX_ITEMS_MAX){
		printf("[%s] param[reversDisplayIndex] error:%d\r\n", __FUNCTION__, reversDisplayIndex);
		return -1;
	}

    uint8_t i = 0;
    uint8_t fs = 0;
    for(i = 0; i < PAGE_MAX_LINE && NULL != *(startItemIndex + i); i++){
        if(reversDisplayIndex == startItemIndex + i)
            fs = 1;
        if(refreshDDRAMLine(i, *(startItemIndex + i), fs)){
            printf("refreshDDRAMLine failed\r\n");
            printf("Times:%d\r\n", i+1);
            return -1;
        }
    }
    return 0;
}
#endif

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

    menu_obj_t *rootMenuObj = createMenuObj(rootMenuItemList, MENU_STATE_ACTIVE);
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
    return 0;
}
