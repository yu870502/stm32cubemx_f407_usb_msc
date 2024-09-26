#ifndef __SIMPLE_UI__
#define __SIMPLE_UI__

#include "main.h"
#include "cmsis_os.h"
#include "event_groups.h"

#define PAGE_MAX_LINE 4
#define ITEM_CONTENT_LEN_MAX 16

//: As a delimiter
#define ITEM_TITLE_DELIMITER    ':'
// for example
// #define ITEM_TITLE_VAR_MCU_TEMP "TEMP:%0.2f"


#define MENU_STATE_SLEEP 0
#define MENU_STATE_ACTIVE 1

#define MENU_EVENT_NONE 0
#define MENU_EVENT_CLOCKWISE 0x01
#define MENU_EVENT_ANTICCLOCKWISE 0x02
#define MENU_EVENT_CLICK 0x04
#define MENU_EVENT_DOUBLE_CLICK 0x08

#define MENU_ITME_UP_SCROLL 0
#define MENU_ITME_DOWN_SCROLL 1

typedef struct __menu_object menu_obj_t;
typedef struct __item_obj item_obj_t;

typedef enum __item_type_t{
    ITEM_TYPE_TITLE = 0,
    ITEM_TYPE_TITLE_VARIABLE,
    ITEM_TYPE_MAX,
}itemType_t;

typedef enum __item_mode_t{
    ITEM_MODE_VIEW = 0,
    ITEM_MODE_CONFIG_VAR,
    ITEM_MODE_MAX,
}itemMode_t;

typedef enum __menu_mode_t{
    MENU_MODE_VIEW = 0,
    MENU_MODE_BACKGROUND,
    MENU_MODE_MAX,
}menuMode_t;

typedef int8_t (*menuEventProcess_t)(menu_obj_t *, uint8_t *);
typedef struct __menu_object
{
    char *menuName;    
    uint8_t state;              //active or sleep
    menuMode_t menuMode;

    uint8_t itemTotal;
    item_obj_t *itemHead;
    uint8_t selectedItemIndex;  //The selected item needs to be displayed in reverse
    uint8_t currentPageIndex;
    menuEventProcess_t menuEventProcess;
}menu_obj_t;

#define VARIABLE_VALUE_TYPE_INT 0
#define VARIABLE_VALUE_TYPE_FLOAT 1
#define VARIABLE_VALUE_TYPE_CHAR 2

typedef struct __item_variable_t{
    int8_t type;
    uint8_t len;
    void *val;
}item_variable_t;

typedef int8_t ( *ItemEventProcess_t)(uint8_t event, item_variable_t* preVar);
typedef int8_t ( *readItemVar_val_t)(item_variable_t **var);

typedef struct __item_obj
{
    itemType_t type;
    itemMode_t mode;
    uint8_t itemIndex;
    char itemTitle[ITEM_CONTENT_LEN_MAX + 1];
    item_variable_t *preVar;
    readItemVar_val_t readItemVar_val;
    ItemEventProcess_t ItemEventProcess;
    struct __item_obj *nextItem;
    menu_obj_t *parentMenu;
}item_obj_t;

extern EventGroupHandle_t xMenuEventGrp;

int8_t clearUILine(uint8_t line);
int8_t refreshUILine(uint8_t line, const char *cont, unsigned int reverse);
int8_t refreshMenuPage(menu_obj_t *menu);

menu_obj_t *getCurrentMenu(void);
item_obj_t *matchItem_selected(menu_obj_t *menu);
item_obj_t *matchItem_Name(menu_obj_t *menu, const char *itemName);
int8_t menuItemScroll(menu_obj_t *menu, uint8_t scroll);
int8_t nowSlectedItemToggleMode(menu_obj_t *menu);

menu_obj_t *createMenu(const char *menuName, menuEventProcess_t menuEventProcess);
item_obj_t *createItem(itemType_t type, const char *title, readItemVar_val_t readItemVar_val, ItemEventProcess_t ItemEventProcess);
int8_t addItemToMenu(menu_obj_t *menu, item_obj_t *item);

int8_t startMenuEventProcess(void);
int8_t startMenu(menu_obj_t *menu);

#endif
