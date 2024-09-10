#include "main.h"
#include "simpleUI.h"

#include "simple_log.h"

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
        }break;

        case MENU_EVENT_ANTICCLOCKWISE: //菜单上滚动封装成api
        if(rootMenuObj->selectedItemIndex > 0){
            rootMenuObj->selectedItemIndex--;
            if(rootMenuObj->selectedItemIndex < rootMenuObj->currentPageIndex){
                rootMenuObj->currentPageIndex--;
            }
        }break;

        case MENU_EVENT_CLICK:
        break;
        case MENU_EVENT_DOUBLE_CLICK:
        break;

        default:
            printf("Unknow menu event:%d\r\n", *event);break;
    }
    printf("Menu name:%s\r\n", rootMenuObj->menuName);
    printf("selectedItem:%d\r\n", rootMenuObj->selectedItemIndex);
    printf("currentPage:%d\r\n\r\n", rootMenuObj->currentPageIndex);

    refreshUIMenu(rootMenuObj);
    return 0;
}

//----------------------------------sub1 menu---------------------------------------

//----------------------------------sub2 menu---------------------------------------

//----------------------------------subn menu---------------------------------------

int8_t userMenuInit(void)
{
    if(creatMenuItemList(&rootMenuItemList, rootMenuItemTittleGrp)){
        LOG_EOR("creatMenuItemList failed");
        return -1;
    }
    rootMenuObj = createMenuObj("Root menu", rootMenuItemList, MENU_STATE_ACTIVE, rootMenuEventProcess);
    if(!rootMenuObj){
        LOG_EOR("createMenuObj failed");
        return -1;
    }
    if(refreshUIMenu(rootMenuObj)){
        LOG_EOR("refreshUIMenu failed");
        return -1;
    }

    startMenuEventProcess();

    //TODO:After all object creation failures, a cleanup action must be performed

    return 0;
}
