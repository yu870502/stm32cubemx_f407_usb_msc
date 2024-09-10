#ifndef __SIMPLE_LOG__
#define __SIMPLE_LOG__

#include <stdio.h>

#define COLOR_RED "\x1b[31m"
#define COLOR_GREEN "\x1b[32m"
#define COLOR_YELLOW "\x1b[33m"
#define COLOR_RESET "\x1b[0m"

#define TAG_E_FUNCTION_LINE "[E][%s][%d]"
#define TAG_I_FUNCTION_LINE "[I][%s][%d]"
#define TAG_D_FUNCTION_LINE "[D][%s][%d]"
#define TAG_W_FUNCTION_LINE "[W][%s][%d]"

#define LOG_LEVEL_NONE 0
#define LOG_LEVEL_ERROR 1
#define LOG_LEVEL_INFO 2
#define LOG_LEVEL_WARNING 3
#define LOG_LEVEL_DEBUG 4

#define CURRENT_LOG_LEVEL LOG_LEVEL_DEBUG // 设置默认日志等级为 DEBUG

// 根据日志等级输出日志
#define LOG_EOR(fmt, ...) if (LOG_LEVEL_ERROR <= CURRENT_LOG_LEVEL) printf(COLOR_RED TAG_E_FUNCTION_LINE fmt COLOR_RESET "\r\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOG_IN(fmt, ...) if (LOG_LEVEL_INFO <= CURRENT_LOG_LEVEL) printf(COLOR_GREEN TAG_I_FUNCTION_LINE fmt COLOR_RESET "\r\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOG_DB(fmt, ...) if (LOG_LEVEL_DEBUG <= CURRENT_LOG_LEVEL) printf(COLOR_RESET TAG_D_FUNCTION_LINE fmt COLOR_RESET "\r\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#define LOG_WA(fmt, ...) if (LOG_LEVEL_WARNING <= CURRENT_LOG_LEVEL) printf(COLOR_YELLOW TAG_W_FUNCTION_LINE fmt COLOR_RESET "\r\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)

#endif
