/*
 * Copyright (c) 2019 Coopermaq
 * All rights reserved
 */
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "FreeRTOS.h"
#include "usart.h"

#include "log.h"

#define UART_BUFFER 2048

void uart6_printf(const char* fmt, ...) {
    char buf[UART_BUFFER] = {0};
    int len = 0;

    va_list arg;
    va_start (arg, fmt);
    vsnprintf(buf, UART_BUFFER, fmt, arg);
    va_end (arg);

    len = strlen(buf);
    HAL_UART_Transmit(&huart6, (uint8_t*)buf, len, 5000);
}

