#ifndef __SIMPLE_OBJ_H__
#define __SIMPLE_OBJ_H__

#include "stdlib.h"

#include "simple_log.h"

#define OBJ_LOG_EORR LOG_EOR
#define OBJ_LOG_WARN LOG_EOR
#define OBJ_LOG_INFO LOG_EOR
#define OBJ_LOG_EOR_DBG LOG_EOR

#define IS_OBJ_EXIST(obj)\
    if(obj){\
        OBJ_LOG_WARN(#obj" already exist");\
        return 1;\
    }

#define IS_OBJ_NOT_EXIST(obj)\
    if(!obj){\
        OBJ_LOG_WARN(#obj" do not exist");\
        return -1;\
    }

#define CREATE_OBJ(obj, objTyep)\
    IS_OBJ_EXIST(obj)\
    if(NULL == (obj = calloc(1, sizeof(objTyep)))){ \
        LOG_EOR("Create " #obj " failed");\
        return -1;\
    }

#define FREE_OBJ(obj)\
    IS_OBJ_NOT_EXIST(obj)\
    free(obj);\
    obj = NULL;\

#endif
