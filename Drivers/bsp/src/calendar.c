/*
 * Copyright (c) 2019 Coopermaq
 * All rights reserved
 */

#include <string.h>
#include <stdio.h>
#include "rtc.h"

#include "calendar.h"

uint8_t adjust_time(uint8_t hour, uint8_t min, uint8_t sec) {
	RTC_TimeTypeDef time = { 0 };

	time.Hours = hour;
	time.Minutes = min;
	time.Seconds = sec;
	time.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	time.StoreOperation = RTC_STOREOPERATION_RESET;
	if (HAL_RTC_SetTime(&hrtc, &time, RTC_FORMAT_BIN) != HAL_OK) {
		Error_Handler();
		return HAL_ERROR;
	}

	return HAL_OK;
}

uint8_t get_time(uint8_t *hour, uint8_t *min, uint8_t *sec) {
	RTC_TimeTypeDef time;

	HAL_RTC_GetTime(&hrtc, &time, RTC_FORMAT_BIN);
	*hour = time.Hours;
	*min = time.Minutes;
	*sec = time.Seconds;

	return HAL_OK;
}

uint8_t get_formatted_time(uint8_t *hours, uint8_t *minutes, uint8_t *seconds,
		char *str_time) {
	char str_time_result[10];
	char str_minutes[3];
	char str_seconds[3];

	get_time(hours, minutes, seconds);

	if (*hours < 10) {
		sprintf(str_time_result, "0%u", *hours);
	} else {
		sprintf(str_time_result, "%u", *hours);
	}
	if (*minutes < 10) {
		sprintf(str_minutes, "0%u", *minutes);
	} else {
		sprintf(str_minutes, "%u", *minutes);
	}
	if (*seconds < 10) {
		sprintf(str_seconds, "0%u", *seconds);
	} else {
		sprintf(str_seconds, "%u", *seconds);
	}

	strcat(str_time_result, ":");
	strcat(str_time_result, str_minutes);
	strcat(str_time_result, ":");
	strcat(str_time_result, str_seconds);
	strcpy(str_time, str_time_result);
	return HAL_OK;
}

uint8_t adjust_date(uint8_t year, uint8_t day, uint8_t month, uint8_t week_day) {
	RTC_DateTypeDef date = { 0 };

	date.WeekDay = week_day;
	date.Month = month;
	date.Date = day;
	date.Year = year;

	if (HAL_RTC_SetDate(&hrtc, &date, RTC_FORMAT_BIN) != HAL_OK) {
		Error_Handler();
		return HAL_ERROR;
	}

	return HAL_OK;
}

uint8_t get_date(uint8_t *year, uint8_t *day, uint8_t *month, uint8_t *week_day) {
	RTC_DateTypeDef date = { 0 };

	HAL_RTC_GetDate(&hrtc, &date, RTC_FORMAT_BIN);
	*month = date.Month;
	*day = date.Date;
	*year = date.Year;
	*week_day = date.WeekDay;

	return HAL_OK;
}

uint8_t get_formatted_date(uint8_t *year, uint8_t *day, uint8_t *month,
		uint8_t *week_day, char *str_date) {
	char str_date_result[10];
	char str_year[3];
	char str_month[3];

	get_date(year, day, month, week_day);

	if (*day < 10) {
		sprintf(str_date_result, "0%u", *day);
	} else {
		sprintf(str_date_result, "%u", *day);
	}
	if (*month < 10) {
		sprintf(str_month, "0%u", *month);
	} else {
		sprintf(str_month, "%u", *month);
	}
	if (*year < 10) {
		sprintf(str_year, "0%u", *year);
	} else {
		sprintf(str_year, "%u", *year);
	}

	strcat(str_date_result, "/");
	strcat(str_date_result, str_month);
	strcat(str_date_result, "/");
	strcat(str_date_result, str_year);

	strcpy(str_date, str_date_result);

	return HAL_OK;
}
