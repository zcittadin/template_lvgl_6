/*
 * Copyright (c) 2019 Coopermaq
 * All rights reserved
 */

#ifndef CALENDAR_H
#define CALENDAR_H

#include "stm32f4xx_hal.h"

uint8_t adjust_time(uint8_t hour, uint8_t min, uint8_t sec);
uint8_t get_time(uint8_t *hour, uint8_t *min, uint8_t *sec);
uint8_t get_formatted_time(uint8_t *hours, uint8_t *minutes, uint8_t *seconds, char *str_time);
uint8_t adjust_date(uint8_t year, uint8_t day, uint8_t month, uint8_t week_day);
uint8_t get_date(uint8_t *year, uint8_t *day, uint8_t *month, uint8_t *week_day);
uint8_t get_formatted_date(uint8_t *year, uint8_t *day, uint8_t *month, uint8_t *week_day, char *str_date);

#endif /* CALENDAR_H */
