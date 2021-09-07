/*
 * Copyright (c) 2019 Coopermaq
 * All rights reserved
 */

#ifndef _LOG_H_
#define _LOG_H_

#include <string.h>

void uart6_printf(const char* fmt, ...);

//#define __FILENAME__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)

#ifdef LOG_DEBUG_FLAG
    #define LOG_DEBUG(fmt, ...) uart6_printf("[DBG][%s:%d %s] "fmt, __FILENAME__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#else
    #define LOG_DEBUG(fmt, ...)
#endif

#ifdef LOG_DEBUG2_FLAG
    #define LOG_DEBUG2(fmt, ...) uart6_printf("[DBG][%s:%d %s] "fmt, __FILENAME__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#else
    #define LOG_DEBUG2(fmt, ...)
#endif

#ifdef LOG_INFO_FLAG
    #define LOG_INFO(fmt, ...) uart6_printf("%s[INF] "fmt, ##__VA_ARGS__)
#elif LOG_INFO_FLAG_ALL
    #define LOG_INFO(fmt, ...) uart6_printf("[INF][%s:%d %s] "fmt, __FILENAME__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#else
    #define LOG_INFO(fmt, ...)
#endif

#ifdef LOG_WARN_FLAG
    #define LOG_WARN(fmt, ...) uart6_printf("[WARN][%s:%d %s] "fmt, __FILENAME__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#else
    #define LOG_WARN(fmt, ...)
#endif

#ifdef LOG_ERROR_FLAG
    #define LOG_ERROR(fmt, ...) uart6_printf("[ERR][%s:%d %s] "fmt, __FILENAME__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#else
    #define LOG_ERROR(fmt, ...)
#endif

#endif /* _LOG_H_ */
