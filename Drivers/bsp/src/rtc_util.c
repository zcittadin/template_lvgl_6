/*
 * Copyright (c) 2020 Coopermaq
 * All rights reserved
 * Author: Zander Cittadin
 */

#include <string.h>
#include <stdio.h>
#include <string.h>
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"
#include "rtc.h"
#include "log.h"
#include "rtc_util.h"

static void format_date(formatted_date_time_t *formatted_date);
static void format_time(formatted_date_time_t *formatted_time);

static formatted_date_time_t formatted_date_time;

static RTC_DateTypeDef rtc_date;
static RTC_TimeTypeDef rtc_time;

TaskHandle_t rtc_handle = NULL;
SemaphoreHandle_t rtc_mutex = NULL;

uint8_t adjust_date(RTC_DateTypeDef *date) {
	if (HAL_RTC_SetDate(&hrtc, date, RTC_FORMAT_BIN) != HAL_OK) {
		Error_Handler();
		return HAL_ERROR;
	}
	return HAL_OK;
}

uint8_t get_date(RTC_DateTypeDef *date) {
	if(HAL_RTC_GetDate(&hrtc, date, RTC_FORMAT_BIN) != HAL_OK) {
		Error_Handler();
		return HAL_ERROR;
	}
	return HAL_OK;
}

static void format_date(formatted_date_time_t *formatted_date) {
	char str_date_result[10];
	char str_year[3];
	char str_month[3];

	get_date(&rtc_date);

	if(rtc_date.Date < 10) {
		sprintf(str_date_result, "0%u", rtc_date.Date);
	} else {
		sprintf(str_date_result, "%u", rtc_date.Date);
	}
	if(rtc_date.Month < 10) {
		sprintf(str_month, "0%u", rtc_date.Month);
	} else {
		sprintf(str_month, "%u", rtc_date.Month);
	}
	if(rtc_date.Year < 10) {
		sprintf(str_year, "0%u", rtc_date.Year);
	} else {
		sprintf(str_year, "%u", rtc_date.Year);
	}

	strcat(str_date_result, "/");
	strcat(str_date_result, str_month);
	strcat(str_date_result, "/");
	strcat(str_date_result, str_year);
	strcpy(formatted_date->formatted_date, str_date_result);
}

uint8_t adjust_time(RTC_TimeTypeDef *time) {
	time->DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	time->StoreOperation = RTC_STOREOPERATION_RESET;
	if (HAL_RTC_SetTime(&hrtc, time, RTC_FORMAT_BIN) != HAL_OK) {
		Error_Handler();
		return HAL_ERROR;
	}
	return HAL_OK;
}

uint8_t get_time(RTC_TimeTypeDef *time) {
	if (HAL_RTC_GetTime(&hrtc, time, RTC_FORMAT_BIN) != HAL_OK) {
		Error_Handler();
		return HAL_ERROR;
	}
	return HAL_OK;
}


static void format_time(formatted_date_time_t *formatted_time) {
	char str_time_result[10];
	char str_minutes[3];
	char str_seconds[3];

	get_time(&rtc_time);

	if (rtc_time.Hours < 10) {
		sprintf(str_time_result, "0%u", rtc_time.Hours);
	} else {
		sprintf(str_time_result, "%u", rtc_time.Hours);
	}
	if (rtc_time.Minutes < 10) {
		sprintf(str_minutes, "0%u", rtc_time.Minutes);
	} else {
		sprintf(str_minutes, "%u", rtc_time.Minutes);
	}
	if (rtc_time.Seconds < 10) {
		sprintf(str_seconds, "0%u", rtc_time.Seconds);
	} else {
		sprintf(str_seconds, "%u", rtc_time.Seconds);
	}

	strcat(str_time_result, ":");
	strcat(str_time_result, str_minutes);
	strcat(str_time_result, ":");
	strcat(str_time_result, str_seconds);
	strcpy(formatted_time->formatted_time, str_time_result);
}

uint8_t get_formatted_date_time(formatted_date_time_t *data) {
	if (xSemaphoreTake(rtc_mutex, pdMS_TO_TICKS(10)) != pdTRUE) {
		return HAL_ERROR;
	}
	strcpy(data->formatted_date, formatted_date_time.formatted_date);
	strcpy(data->formatted_time, formatted_date_time.formatted_time);
	xSemaphoreGive(rtc_mutex);
	return HAL_OK;
}

void v_task_rtc(void * pvParameters) {
	while (1) {
		if (xSemaphoreTake(rtc_mutex, pdMS_TO_TICKS(10)) != pdTRUE) {
			return;
		}
		format_time(&formatted_date_time);
		format_date(&formatted_date_time);
		xSemaphoreGive(rtc_mutex);
		osDelay(100);
	}
	vTaskDelete(rtc_handle);
	rtc_handle = NULL;
	while (1) { }
}

uint8_t rtc_init() {
	BaseType_t ret;
	if(!rtc_mutex)
		rtc_mutex = xSemaphoreCreateMutex();
	ret = xTaskCreate(v_task_rtc, "v_task_rtc", configMINIMAL_STACK_SIZE, NULL,
		tskIDLE_PRIORITY, &rtc_handle);
	if (ret != pdPASS) {
		return HAL_ERROR;
	}
	return HAL_OK;
}

