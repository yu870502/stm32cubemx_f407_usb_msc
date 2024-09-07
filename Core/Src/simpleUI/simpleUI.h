#ifndef __SIMPLE_UI__
#define __SIMPLE_UI__

#include "main.h"

#define MENU_MAX_LINE 4

#define MENU_INDEX_MCU_TEMP "1"
#define MAIN_MENU_ITEM_MCU_TEMP "Curr temp:%f"

#define MENU_INDEX_SET_SCR_EV "2"
#define MAIN_MENU_ITEM_SET_SCREEN_EV "SCR EV:%d"

#define MENU_INDEX_SCR_LEDA "3"
#define MAIN_MENU_ITEM_SCR_BL "SCR BL:%d"

#define MENU_INDEX_LOG "4"
#define MAIN_MENU_ITEM_LOG "LOG:%s"

#define MENU_INDEX_FW_VERSION "5"
#define MAIN_MENU_ITEM_FW_VERSION "FW ver:%s"

typedef struct __menu_object
{
    uint8_t selectedMenuItemIndex;  //The selected item in the menu need to be displayed in reverse
    uint8_t selectedMenuItemLine;   //The selected item is located at the line number on the page
}menu_obj_t;

int8_t clearUILine(uint8_t line);
int8_t refreshUILine(uint8_t line, char *cont, uint8_t fs);
void refreshMainMenuTest(void);

menu_obj_t *createMenuObject(void);
menu_obj_t *getMenuObject(void);

#endif
