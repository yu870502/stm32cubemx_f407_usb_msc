#include <string.h>

#include "main.h"
#include "simpleUI.h"
#include "simpleUserUI.h"

#include "simple_log.h"
#include "simple_obj.h"

#include "mcu_obj.h"
//----------------------------------define item---------------------------------------
int8_t loadItemVar_mcuTemp(item_obj_t *item)
{
    IS_NULL(item, -1);

    mcu_obj_t *mcu;
    if(NULL == (mcu = getMcuObj())){
        LOG_EOR("getMcuObj is NULL");
        return -1;
    }
    float temp = 0.0f;
    if(mcu->getMcuTemp(&temp)){
        LOG_EOR("getMcuTemp failed");
        return -1;
    }
    
    memset(item->itemTitle, 0, sizeof(item->itemTitle));
    snprintf(item->itemTitle, ITEM_TITLE_MAX, ITEM_TITLE_VAR_MCU_TEMP, temp);

    return 0;
}
int8_t loadItemVar_scrRefreshRate(item_obj_t *item)
{
    IS_NULL(item, -1);
    return 0;
}
int8_t loadItemVar_scrEV(item_obj_t *item)
{
    IS_NULL(item, -1);
    return 0;
}
int8_t loadItemVar_scrBL(item_obj_t *item)
{
    IS_NULL(item, -1);
    return 0;
}
int8_t loadItemVar_FW_VER(item_obj_t *item)
{
		IS_NULL(item, -1);
    return 0;
}

menu_obj_t *rootMenu = NULL;
int8_t rootMenuEventProcess(uint8_t *event)
{
    IS_NULL(event, -1);

    switch(*event){
        case MENU_EVENT_CLOCKWISE:
            menuItemScroll(rootMenu, MENU_ITME_DOWN_SCROLL);
        break;

        case MENU_EVENT_ANTICCLOCKWISE:
            menuItemScroll(rootMenu, MENU_ITME_UP_SCROLL);
        break;

        case MENU_EVENT_CLICK:
            LOG_DB("click event");
            menuItemToggleMode(rootMenu);
        break;
        case MENU_EVENT_DOUBLE_CLICK:
        break;

        default:
            printf("Unknow menu event:%d\r\n", *event);break;
    }
    printf("Menu name:%s\r\n", rootMenu->menuName);
    printf("selectedItem:%d\r\n", rootMenu->selectedItemIndex);
    printf("currentPage:%d\r\n\r\n", rootMenu->currentPageIndex);

    refreshMenuPage(rootMenu);
    return 0;
}

//----------------------------------sub1 menu---------------------------------------

//----------------------------------sub2 menu---------------------------------------

//----------------------------------subn menu---------------------------------------

void printItem(menu_obj_t *menu){
    if(!(menu->itemHead)){
        LOG_WA("menu is NULL");
        return;
    }
    item_obj_t *m = menu->itemHead;
    while(m){
        LOG_DB("item[%d] title:%s", m->itemIndex, m->itemTitle);
        m = m->nextItem;
    }
}


int8_t userMenuInit(void)
{
    rootMenu = createMenu("Root menu", rootMenuEventProcess);
    IS_NULL(rootMenu, -1);

    item_obj_t *item = createItem(ITEM_TYPE_TITLE_VARIABLE, ITEM_TITLE_VAR_MCU_TEMP, loadItemVar_mcuTemp, NULL);
    IS_NULL(item, -1);
    if(addItemToMenu(rootMenu, item)){
        LOG_EOR("addItemToMenu failed");
        return -1;
    }

    item = createItem(ITEM_TYPE_TITLE_VARIABLE, ITEM_TITEL_VAR_SCR_REFRESH_RATE, loadItemVar_scrRefreshRate, NULL);
    IS_NULL(item, -1);
    if(addItemToMenu(rootMenu, item)){
        LOG_EOR("addItemToMenu failed");
        return -1;
    }

    item = createItem(ITEM_TYPE_TITLE_VARIABLE, ITEM_TITEL_VAR_SCR_EV, loadItemVar_scrEV, NULL);
    IS_NULL(item, -1);
    if(addItemToMenu(rootMenu, item)){
        LOG_EOR("addItemToMenu failed");
        return -1;
    }
    item = createItem(ITEM_TYPE_TITLE_VARIABLE, ITEM_TITLE_VAR_SCR_BL, loadItemVar_scrBL, NULL);
    IS_NULL(item, -1);
    if(addItemToMenu(rootMenu, item)){
        LOG_EOR("addItemToMenu failed");
        return -1;
    }
    item = createItem(ITEM_TYPE_TITLE, ITEM_TITLE_GONGDE, NULL, NULL);
    IS_NULL(item, -1);
    if(addItemToMenu(rootMenu, item)){
        LOG_EOR("addItemToMenu failed");
        return -1;
    }  
    item = createItem(ITEM_TYPE_TITLE_VARIABLE, ITEM_TITLE_VAR_FW_VER, loadItemVar_FW_VER, NULL);
    IS_NULL(item, -1);
    if(addItemToMenu(rootMenu, item)){
        LOG_EOR("addItemToMenu failed");
        return -1;
    }
    printItem(rootMenu);

    startMenu(rootMenu);

    return 0;
}
