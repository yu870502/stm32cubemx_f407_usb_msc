#include <string.h>
#include <stdio.h>
#include <string.h>

#include "main.h"
#include "simple_menu.h"
#include "simpleUserUI.h"

#include "simple_log.h"
#include "simple_obj.h"
#include "simple_scr.h"

#include "mcu_obj.h"
#include "app.h"

//----------------------------------define item---------------------------------------
int8_t eventProcess_scrRefreshRate(uint8_t event, item_variable_t *preVar)
{
    IS_NULL(preVar, -1);
    IS_NULL(preVar->val, -1);

    uint8_t *v = (uint8_t *)(preVar->val);
    switch(event){
        case MENU_EVENT_CLOCKWISE:
            if(*v + 1 <= SCR_REFRESH_RATE_MAX){
                *v += 1;
            }break;

        case MENU_EVENT_ANTICCLOCKWISE:
            if(*v - 1 >= 0){
                *v -= 1;
            }break;

        case MENU_EVENT_CLICK:
            scrObj.scrRefreshRate = *v;
            break;

        default:
            LOG_EOR("Unknow event");
            break;
    }
    return 0;
}

int8_t eventProcess_scrBL(uint8_t event, item_variable_t *preVar)
{
    IS_NULL(preVar, -1);
    IS_NULL(preVar->val, -1);

    char *v = (char *)(preVar->val);
    switch(event){
        case MENU_EVENT_CLOCKWISE:
        case MENU_EVENT_ANTICCLOCKWISE:
            if(strstr(v, SCR_BL_OFF)){
                memset(v, 0, preVar->len);
                strncpy(v, SCR_BL_ON, preVar->len);
            }
            else{
                memset(v, 0, preVar->len);
                strncpy(v, SCR_BL_OFF, preVar->len);
            }break;
        case MENU_EVENT_CLICK:
            strncpy(scrObj.scrBl, v, preVar->len);
            break;

        default:
            LOG_EOR("Unknow event");
            break;
    }
    return 0;
}

int8_t read_mcu_temp(item_variable_t **var)
{
    IS_NULL(var, -1);

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

    if(NULL == (*var = pvPortMalloc(sizeof(item_variable_t)))){
        LOG_EOR("Alloc var failed");
        return -1; 
    }
    if(NULL == ((*var)->val = pvPortMalloc(sizeof(float)))){
        LOG_EOR("Alloc var->val failed");
        return -1; 
    }
    (*var)->type = VARIABLE_VALUE_TYPE_FLOAT;
    (*var)->len = sizeof(float);
    *((float *)((*var)->val)) = temp;

    return 0;
}
int8_t read_scrRefreshRate(item_variable_t **var)
{
    IS_NULL(var, -1);
    
    if(NULL == (*var = pvPortMalloc(sizeof(item_variable_t)))){
        LOG_EOR("Alloc var failed");
        return -1; 
    }
    if(NULL == ((*var)->val = pvPortMalloc(sizeof(float)))){
        LOG_EOR("Alloc var->val failed");
        return -1; 
    }

    (*var)->type = VARIABLE_VALUE_TYPE_INT;
    (*var)->len = sizeof(uint32_t);   
    *((uint32_t *)((*var)->val)) = (uint32_t)(scrObj.scrRefreshRate);
    return 0;
}
int8_t read_var_scrEV(item_variable_t **var)
{
    IS_NULL(var, -1);

    if(NULL == (*var = pvPortMalloc(sizeof(item_variable_t)))){
        LOG_EOR("Alloc var failed");
        return -1; 
    }
    if(NULL == ((*var)->val = pvPortMalloc(sizeof(float)))){
        LOG_EOR("Alloc var->val failed");
        return -1; 
    }

    (*var)->type = VARIABLE_VALUE_TYPE_INT;
    (*var)->len = sizeof(uint32_t);   
    *((uint32_t *)((*var)->val)) = (uint32_t)(scrObj.scrEV);

    return 0;
}
int8_t read_var_scrBL(item_variable_t **var)
{
    IS_NULL(var, -1);

    if(NULL == (*var = pvPortMalloc(sizeof(item_variable_t)))){
        LOG_EOR("Alloc var failed");
        return -1; 
    }
    if(NULL == ((*var)->val = pvPortMalloc(sizeof(scrObj.scrBl)))){
        LOG_EOR("Alloc var->val failed");
        return -1; 
    }

    (*var)->type = VARIABLE_VALUE_TYPE_CHAR;
    (*var)->len = sizeof(scrObj.scrBl);
    snprintf((*var)->val, (*var)->len, "%s", scrObj.scrBl);
    return 0;
}
int8_t read_var_GongDe(item_variable_t **var)
{
    IS_NULL(var, -1);
    *var = NULL;
    return 0;
}
int8_t read_var_FW_VER(item_variable_t **var)
{
    IS_NULL(var, -1);
    *var = NULL;
    
    if(NULL == (*var = pvPortMalloc(sizeof(item_variable_t)))){
        LOG_EOR("Alloc var failed");
        return -1; 
    }
    if(NULL == ((*var)->val = pvPortMalloc(strlen(APP_VERSION + 1)))){
        LOG_EOR("Alloc var->val failed");
        return -1; 
    }

    (*var)->type = VARIABLE_VALUE_TYPE_CHAR;
    (*var)->len = strlen(APP_VERSION) + 1;
    snprintf((*var)->val, (*var)->len, "%s", APP_VERSION);
    return 0;
}

menu_obj_t *rootMenu = NULL;
int8_t menuEventProcess(menu_obj_t *menu, uint8_t *event)
{
    IS_NULL(menu, -1);
    IS_NULL(event, -1);

    switch(*event){
        case MENU_EVENT_CLOCKWISE:
            menuItemScroll(menu, MENU_ITME_DOWN_SCROLL);
        break;

        case MENU_EVENT_ANTICCLOCKWISE:
            menuItemScroll(menu, MENU_ITME_UP_SCROLL);
        break;
    }
    return 0;
}

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
    rootMenu = createMenu("Root menu", menuEventProcess);
    IS_NULL(rootMenu, -1);

    item_obj_t *item = createItem(ITEM_TYPE_TITLE, ITEM_TITLE_VAR_MCU_TEMP, read_mcu_temp, NULL);
    IS_NULL(item, -1);
    if(addItemToMenu(rootMenu, item)){
        LOG_EOR("addItemToMenu failed");
        return -1;
    }

    item = createItem(ITEM_TYPE_TITLE_VARIABLE, ITEM_TITEL_VAR_SCR_REFRESH_RATE, read_scrRefreshRate, eventProcess_scrRefreshRate);
    IS_NULL(item, -1);
    if(addItemToMenu(rootMenu, item)){
        LOG_EOR("addItemToMenu failed");
        return -1;
    }

    item = createItem(ITEM_TYPE_TITLE_VARIABLE, ITEM_TITEL_VAR_SCR_EV, read_var_scrEV, NULL);
    IS_NULL(item, -1);
    if(addItemToMenu(rootMenu, item)){
        LOG_EOR("addItemToMenu failed");
        return -1;
    }
    item = createItem(ITEM_TYPE_TITLE_VARIABLE, ITEM_TITLE_VAR_SCR_BL, read_var_scrBL, eventProcess_scrBL);
    IS_NULL(item, -1);
    if(addItemToMenu(rootMenu, item)){
        LOG_EOR("addItemToMenu failed");
        return -1;
    }
    item = createItem(ITEM_TYPE_TITLE, ITEM_TITLE_GONGDE, read_var_GongDe, NULL);
    IS_NULL(item, -1);
    if(addItemToMenu(rootMenu, item)){
        LOG_EOR("addItemToMenu failed");
        return -1;
    }  
    item = createItem(ITEM_TYPE_TITLE, ITEM_TITLE_VAR_FW_VER, read_var_FW_VER, NULL);
    IS_NULL(item, -1);
    if(addItemToMenu(rootMenu, item)){
        LOG_EOR("addItemToMenu failed");
        return -1;
    }
    printItem(rootMenu);

    startMenu(rootMenu);

    return 0;
}
