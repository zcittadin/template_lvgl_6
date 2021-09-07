/*
 * Copyright (c) 2019 Coopermaq
 * All rights reserved
 */

#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"
#include "buzzer.h"
#include "gpio.h"

xTimerHandle buzzer_timer = NULL;
void timer_buzzer(TimerHandle_t xTimer);

void timer_buzzer(TimerHandle_t xTimer) {
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
  buzzer_timer = NULL;
  xTimerDelete(xTimer,0);
}

uint8_t buzzer(uint32_t time_ms) {
  BaseType_t ret;
  if (buzzer_timer) return HAL_ERROR;
  buzzer_timer = xTimerCreate("buzzer", pdMS_TO_TICKS(time_ms), pdFALSE, NULL, timer_buzzer);
  ret = xTimerStart(buzzer_timer, 0 );

  if (ret == pdPASS) {
    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
    return HAL_OK;
  }

  return HAL_ERROR;
}

