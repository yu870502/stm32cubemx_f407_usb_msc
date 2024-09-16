#ifndef __SIMPLE_OBJ_H__
#define __SIMPLE_OBJ_H__

#include "stdlib.h"
#include "stdlib.h"

#include "simple_log.h"

#define OBJ_LOG_ERR LOG_EOR
#define OBJ_LOG_WARN LOG_EOR
#define OBJ_LOG_INFO LOG_EOR
#define OBJ_LOG_EOR_DBG LOG_EOR

#define IS_NULL(target, warn)\
do{\
    if(!target){\
        OBJ_LOG_ERR(#target " is null");\
        return warn;\
    }\
}while(0)

#define IS_OBJ_EXIST(obj, warn)\
do{\
    if(obj){\
        OBJ_LOG_WARN(#obj " already exist");\
        return warn;\
    }\
}while(0)

#define IS_OBJ_NOT_EXIST(obj, warn)\
do{\
    if(!obj){\
        OBJ_LOG_WARN(#obj " do not exist");\
        return warn;\
    }\
}while(0)

#define CREATE_OBJ(obj, objType, warn)\
do{\
    if(NULL == (obj = calloc(1, sizeof(objType)))){\
        OBJ_LOG_ERR("Create " #obj " failed");\
        return warn;\
    }\
}while(0)

#define FREE_OBJ(obj, warn)\
do{\
    free(obj);\
    obj = NULL;\
    return warn;\
}while(0)

#endif
