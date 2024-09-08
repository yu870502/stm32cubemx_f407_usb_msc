#ifndef __SIMPLE_UI__
#define __SIMPLE_UI__

#include "main.h"

#define PAGE_MAX_LINE 4

#define MAIN_MENU_ITEM_MCU_TEMP "Curr temp:%f"
#define MAIN_MENU_ITEM_SET_SCREEN_EV "SCR EV:%d"
#define MAIN_MENU_ITEM_SCR_BL "SCR BL:%d"
#define MAIN_MENU_ITEM_LOG "LOG:%s"
#define MAIN_MENU_ITEM_GONGDE "GongDe+1"
#define MAIN_MENU_ITEM_FW_VERSION "FW ver:%s"

#define MENU_STATE_SLEEP 0
#define MENU_STATE_ACTIVE 1

typedef struct __menu_item_obj
{
    uint8_t itemIndex;
    char *itenTitle;
    struct __menu_item_obj *nextItem;
    void *subMenu;  //Submenu can be one of the items or menus 
}menu_item_obj_t;

typedef struct __menu_page_obj
{
    uint8_t startItemIndex; 
    uint8_t selectedItemIndex;
}menu_page_obj_t;

typedef struct __menu_object
{
    menu_item_obj_t *itemList;
    uint8_t state;              //active or sleep
    uint8_t currentPageIndex;
    uint8_t selectedItemIndex;  //The selected item needs to be displayed in reverse
}menu_obj_t;

int8_t clearUILine(uint8_t line);
int8_t refreshUILine(uint8_t line, char *cont, uint8_t fs);
void refreshMainMenuTest(void);

menu_obj_t *createMenuObject(void);
menu_obj_t *getMenuObject(void);

int8_t userMenuInit(void);

#endif
