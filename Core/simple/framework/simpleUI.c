
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

int8_t refreshUILine(uint8_t line, const char *cont, unsigned int reverse)
{
    if(line >= PAGE_MAX_LINE){
        LOG_EOR("[%s] para [line] error:%d", __func__, line);
        return -1;
    }
	if(!cont){
		LOG_EOR("[%s] param[cont] NULL", __FUNCTION__);
		return -1;
	}
    uint8_t len = strlen(cont);
    if(len > ITEM_CONTENT_LEN_MAX){
        LOG_WA("Line cont error");
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

item_obj_t *matchItem_selected(menu_obj_t *menu)
{
    IS_OBJ_NOT_EXIST(menu, NULL);
    IS_NULL(menu->itemHead, NULL);

    item_obj_t *item = menu->itemHead;
    while(item){
        if(item->itemIndex == menu->selectedItemIndex){
            return item;
        }
        item = item->nextItem;
    }
    LOG_WA("The selected item was not matched");
    return NULL;
}

item_obj_t *matchItem_Name(menu_obj_t *menu, const char *itemName)
{
    IS_NULL(itemName, NULL);
    IS_NULL(menu, NULL);
    IS_NULL(menu->itemHead, NULL);    

    item_obj_t *item = menu->itemHead;
    while(item){
        if(strstr(item->itemTitle, itemName)){
            return item;
        }
        item = item->nextItem;
    }
    LOG_WA("The item name was not matched");
    return NULL;
}

int8_t menuItemScroll(menu_obj_t *menu, uint8_t scroll)
{
    IS_NULL(menu, -1);
    switch(scroll){
        case MENU_ITME_UP_SCROLL:
            if(menu->selectedItemIndex > 0){
                menu->selectedItemIndex--;
                if(menu->selectedItemIndex < menu->currentPageIndex){
                    menu->currentPageIndex--;
                }
            }break;
        case MENU_ITME_DOWN_SCROLL:
            if(menu->selectedItemIndex + 1 < menu->itemTotal){
                menu->selectedItemIndex++;
                if(menu->selectedItemIndex - menu->currentPageIndex >= PAGE_MAX_LINE){
                    if(menu->currentPageIndex < menu->itemTotal - 1){
                        menu->currentPageIndex++;
                    }
                }
            }break;

        default:
            LOG_EOR("Unknow scroll");break;
    }
    return 0;
}

int8_t nowSlectedItemToggleMode(menu_obj_t *menu)
{
    IS_NULL(menu, -1);
    item_obj_t *item = matchItem_selected(menu);
    IS_NULL(item, -1);

    if(ITEM_TYPE_TITLE_VARIABLE == item->type){
        //TODO:每次项目进入var config模式时，创建item preVar（通过获取真实值，赋值给预显示var），为了后续item event process中调整preVar，实时输出到屏幕上
        //TODO:每次菜单退出var config模式前，销毁item preVar
        if(ITEM_MODE_VIEW == item->mode){
            item->mode = ITEM_MODE_CONFIG_VAR;
            if(item->readItemVar_val){
                item->readItemVar_val(&(item->preVar));
            }
            else{
                LOG_WA("Item[%s] readItemVar_val is NULL", item->itemTitle);
            }
            menu->menuMode = MENU_MODE_BACKGROUND;
        }
        else if(ITEM_MODE_CONFIG_VAR == item->mode){
            item->mode = ITEM_MODE_VIEW;
            if(item->preVar){
                if(item->preVar->val){
                    vPortFree(item->preVar->val);   //clear var
                    item->preVar->val = NULL;
                }
                vPortFree(item->preVar);
                item->preVar = NULL;
            }
            menu->menuMode = MENU_MODE_VIEW;
        }
        else{
            LOG_EOR("item[%s] mode error", item->itemTitle);
            return -1;
        }        
    }
    return 0;
}

int8_t refreshItemLine(uint8_t line, item_obj_t *item, const char *content)
{
    IS_OBJ_NOT_EXIST(item, -1);
    IS_NULL(content, -1);

    if(line >= PAGE_MAX_LINE){
        LOG_EOR("[%s] para [line] error:%d", __func__, line);
        return -1;
    }

    unsigned int reverseMask = 0;
    if(item->itemIndex == item->parentMenu->selectedItemIndex){
        if(ITEM_TYPE_TITLE_VARIABLE == item->type || ITEM_TYPE_TITLE == item->type){
            const char *pos = strchr(content, ITEM_TITLE_DELIMITER);
            uint8_t titleLen = pos - content;
            // LOG_WA("titleLen=%d", titleLen);
            uint8_t i = 0;
            reverseMask = 0;
            if(ITEM_MODE_VIEW == item->mode){
                for(i = 0; i < titleLen; i++){
                    reverseMask |= (0x8000 >> i);
                }
            }
            else{
                uint8_t offset = titleLen + 1;   //title:var
                titleLen = ITEM_CONTENT_LEN_MAX - offset;
                for(i = 0; i < titleLen; i++){
                    reverseMask |= (0x8000 >> offset + i);
                }
            }
        }
        else{
            LOG_WA("Unknow item type");
        }
    }
    else{
        reverseMask = 0;
    }

    refreshUILine(line, content, reverseMask);
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
    char itemContent[ITEM_CONTENT_LEN_MAX + 1] = {0};
    item_variable_t *var = NULL;
    for(i = 0; item && i < PAGE_MAX_LINE; i++){
        if(ITEM_MODE_CONFIG_VAR == item->mode){
            var = item->preVar;
        }
        else if(ITEM_MODE_VIEW == item->mode && item->readItemVar_val){
            item->readItemVar_val(&var);
        }

        if(var){
            if(VARIABLE_VALUE_TYPE_INT == var->type){
                snprintf(itemContent, ITEM_CONTENT_LEN_MAX + 1, item->itemTitle, *((int *)(var->val)));
            }
            else if(VARIABLE_VALUE_TYPE_FLOAT == var->type){
                snprintf(itemContent, ITEM_CONTENT_LEN_MAX + 1, item->itemTitle, *((float *)(var->val)));  
            }
            else if(VARIABLE_VALUE_TYPE_CHAR == var->type){
                snprintf(itemContent, ITEM_CONTENT_LEN_MAX + 1, item->itemTitle, (char *)(var->val));  
            }
            if(ITEM_MODE_VIEW == item->mode){
                if(var->val){
                    vPortFree(var->val);
                    var->val = NULL;
                }
                vPortFree(var);
                var = NULL;
            }
        }
        else{
            snprintf(itemContent, ITEM_CONTENT_LEN_MAX + 1, "%s", item->itemTitle);  
        }
        var = NULL;

        refreshItemLine(i, item, itemContent);
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
void menuEventWaitTask(void *arg)
{
    EventBits_t menuEventGrp;
    uint8_t event;
    for(;;){
        LOG_IN("Watting menuEventGrp");
        menuEventGrp = xEventGroupWaitBits(xMenuEventGrp, MENU_EVENT_CLOCKWISE | MENU_EVENT_ANTICCLOCKWISE | MENU_EVENT_CLICK | MENU_EVENT_DOUBLE_CLICK,
                                        pdTRUE, pdFALSE, portMAX_DELAY);
        event = menuEventGrp & (MENU_EVENT_CLOCKWISE | MENU_EVENT_ANTICCLOCKWISE | MENU_EVENT_CLICK | MENU_EVENT_DOUBLE_CLICK);
        LOG_IN("receive menuEventGrp:%d", menuEventGrp);
        menuEventGrp = 0;

        if(!currentMenu){
            LOG_EOR("currentMenu is NULL");
            goto next;
        }

        if(MENU_MODE_VIEW == currentMenu->menuMode){
            if(!currentMenu->menuEventProcess){
                LOG_EOR("currentMenu->menuEventProcess is NULL");
                goto next;
            }
            else{
                currentMenu->menuEventProcess(currentMenu, &event);
            }
        }
        else if(MENU_MODE_BACKGROUND == currentMenu->menuMode){
            item_obj_t *item = matchItem_selected(currentMenu);
            if(!item){
                LOG_WA("match now selected item failed");
                goto next;
            }
            if(!item->ItemEventProcess){
                LOG_EOR("Item event process is NULL");
            }
            else{
                item->ItemEventProcess(event, item->preVar);
            }
        }
        if(MENU_EVENT_CLICK == event){
            nowSlectedItemToggleMode(currentMenu);   //TODO:如果默认只有两种模式，可以这样切换，如果是多个了模式，则参数要指明具体模式。
        }
        refreshMenuPage(currentMenu);
next:        
        event = MENU_EVENT_NONE;
    }
}

osThreadId menuEventWaitTaskHandle = NULL;
int8_t startMenuEventProcess(void)
{
    const menu_obj_t *menu = NULL;
    if(NULL == (menu = getCurrentMenu())){
        LOG_EOR("getCurrentMenu is NULL");
        return -1;
    }
    if(MENU_STATE_SLEEP == menu->state){
        LOG_EOR("Current dev is in an inactive state");
        return -1; 
    }
    if(menuEventWaitTaskHandle){
        LOG_WA("MenuEventProcess already running");
        return 1;
    }

    if(NULL == (xMenuEventGrp = xEventGroupCreate())){
        LOG_EOR("Create menu event grp failed");
        return -1;
    }
    LOG_IN("startMenuEventProcess");
    osThreadDef(menuEventProcess, menuEventWaitTask, osPriorityAboveNormal, 0, 1024);
    menuEventWaitTaskHandle = osThreadCreate(osThread(menuEventProcess), NULL);
    if(!menuEventWaitTaskHandle){
        LOG_EOR("Create menuEventWaitTask failed");
        return -1;
    }
    return 0;
}

menu_obj_t *createMenu(const char *menuName, menuEventProcess_t menuEventProcess)
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
    menu->menuMode = MENU_MODE_VIEW;
    menu->itemTotal = 0;
    menu->itemHead = NULL;
    menu->currentPageIndex = 0;
    menu->selectedItemIndex = 0;
    menu->menuEventProcess = menuEventProcess;
    if(!menuEventProcess)
        LOG_WA("Menu event process is NULL");

    return menu;
}

item_obj_t *createItem(itemType_t type, const char *title, readItemVar_val_t readItemVar_val, ItemEventProcess_t ItemEventProcess)
{
    IS_NULL(title, NULL);
    // IS_NULL(loadItemVar, NULL);
    // IS_NULL(ItemEventProcess, NULL);

    item_obj_t *item = NULL;
    CREATE_OBJ(item, item_obj_t, NULL);
    item->type = type;
    item->mode = ITEM_MODE_VIEW;
    strncpy(item->itemTitle, title, strlen(title));
    item->readItemVar_val = readItemVar_val;
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

