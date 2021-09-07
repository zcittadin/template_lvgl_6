/*
 * Copyright (c) 2020 Coopermaq
 * All rights reserved
 * Author: Zander Cittadin
 */

#ifndef RTC_INTERFACE_H_
#define RTC_INTERFACE_H_

#include "rtc.h"

typedef struct {
	char formatted_time[10];
	char formatted_date[10];
} formatted_date_time_t;

uint8_t rtc_init(void);
uint8_t adjust_date(RTC_DateTypeDef *date);
uint8_t get_date(RTC_DateTypeDef *date);
uint8_t adjust_time(RTC_TimeTypeDef *time);
uint8_t get_time(RTC_TimeTypeDef *time);
uint8_t get_formatted_date_time(formatted_date_time_t *data);

#endif /* RTC_INTERFACE_H_ */
