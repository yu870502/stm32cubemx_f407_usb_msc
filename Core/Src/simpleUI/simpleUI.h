#ifndef __SIMPLE_UI__
#define __SIMPLE_UI__

#include "main.h"
#include "cmsis_os.h"
#include "event_groups.h"

#define PAGE_MAX_LINE 4

#define MAIN_MENU_ITEM_MCU_TEMP "Curr temp:%f"
#define MAIN_MENU_ITEM_SET_SCREEN_EV "SCR EV:%d"
#define MAIN_MENU_ITEM_SCR_BL "SCR BL:%d"
#define MAIN_MENU_ITEM_LOG "LOG:%s"
#define MAIN_MENU_ITEM_GONGDE "GongDe+1"
#define MAIN_MENU_ITEM_FW_VERSION "FW ver:%s"

#define MENU_STATE_SLEEP 0
#define MENU_STATE_ACTIVE 1

#define MENU_EVENT_NONE 0
#define MENU_EVENT_CLOCKWISE 0x01
#define MENU_EVENT_ANTICCLOCKWISE 0x02
#define MENU_EVENT_CLICK 0x04
#define MENU_EVENT_DOUBLE_CLICK 0x08

typedef struct __menu_item_obj
{
    uint8_t itemIndex;
    char *itenTitle;
    struct __menu_item_obj *nextItem;
    void *subMenu;  //Submenu can be one of the items or menus 
}menu_item_obj_t;

typedef uint8_t (*menuEventProcess_t)(uint8_t *);
typedef struct __menu_object
{
    char *menuName;    
    uint8_t state;              //active or sleep

    uint8_t itemTotal;
    menu_item_obj_t *itemList;
    uint8_t currentPageIndex;   //
    uint8_t selectedItemIndex;  //The selected item needs to be displayed in reverse

    menuEventProcess_t menuEventProcess;
}menu_obj_t;

extern EventGroupHandle_t xMenuEventGrp;

int8_t clearUILine(uint8_t line);
int8_t refreshUILine(uint8_t line, char *cont, uint8_t fs);
void refreshMainMenuTest(void);

menu_obj_t *createMenuObject(void);
menu_obj_t *getMenuObject(void);

int8_t refreshUIMenu(menu_obj_t *menuObj);

int8_t userMenuInit(void);

#endif
