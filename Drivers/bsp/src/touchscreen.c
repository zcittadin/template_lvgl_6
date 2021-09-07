/*
 * Copyright (c) 2019 Coopermaq
 * All rights reserved
 */

#include "stm32f4xx_hal.h"
#include "lv_drv_conf.h"
#include "touchscreen.h"

#define THOUCH_HYSTERESIS 8

static void setup_x_axis(void);
static void setup_y_axis(void);
static uint16_t touch_get_X(void);
static uint16_t touch_get_Y(void);

extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;

uint16_t adcX = 0;
uint16_t adcY = 0;

typedef enum {
    IDLE,
    SET_X,
    GET_X,
    SET_Y,
    GET_Y
} TOUCH_STATES;

volatile TOUCH_STATES state = SET_X;

bool read_touch(lv_indev_drv_t *dvr, lv_indev_data_t * data) {
    static int16_t last_x = 0;
    static int16_t last_y = 0;

    int16_t x, y;

    x = touch_get_X();
    y = touch_get_Y();

    if((x > 0) && (y > 0)) {
        data->point.x = x;
        data->point.y = y;
        last_x = data->point.x;
        last_y = data->point.y;
        data->state = LV_INDEV_STATE_PR;
    } else {
        data->point.x = last_x;
        data->point.y = last_y;
        data->state = LV_INDEV_STATE_REL;
    }
    return false;
}

int16_t ad_touch_handler(void) {
    static uint16_t tempX, tempY;
    uint16_t temp;

    switch(state) {
        case IDLE:
            adcX = 0;
            adcY = 0;
            break;
        case SET_X:
            setup_x_axis();
            tempX = 0;
            state = GET_X;
            break;
        case GET_X:
            HAL_ADC_Start(&hadc2);
            if (HAL_ADC_PollForConversion(&hadc2, 1000) != HAL_OK) {
              break;
            }
            temp = (uint16_t)HAL_ADC_GetValue(&hadc2);
            HAL_ADC_Stop(&hadc2);
            if (temp > (tempX + THOUCH_HYSTERESIS) || tempX > (temp + THOUCH_HYSTERESIS)) {
                tempX =  temp;
                break;
            }
            setup_y_axis();
            tempY = 0;
            state = GET_Y;
            break;
        case GET_Y:
            HAL_ADC_Start(&hadc1);
            if (HAL_ADC_PollForConversion(&hadc1, 1000) != HAL_OK) {
              break;
            }
            temp = (uint16_t)HAL_ADC_GetValue(&hadc1);
            HAL_ADC_Stop(&hadc1);
            if (temp > (tempY + THOUCH_HYSTERESIS) || tempY > (temp + THOUCH_HYSTERESIS)) {
                tempY =  temp;
                break;
            }
            adcX = tempX;
            adcY = tempY;
            setup_x_axis();
            tempX = 0;
            state = GET_X;
            break;

        case SET_Y:
            setup_y_axis();
            tempY = 0;
            state = GET_Y;
            break;
        default:
            state = SET_X;
            return 1; // touch screen acquisition is done
    }
    return 0; // touch screen acquisition is not done
}

static uint16_t touch_get_X(void)
{
    uint16_t result;
    if (adcX < TOUCH_X_MIN)
      return 0;
    result = (uint32_t) ((uint32_t) (adcX - TOUCH_X_MIN) * TOUCH_HOR_RES)
    / (TOUCH_X_MAX - TOUCH_X_MIN);
    if (result > TOUCH_HOR_RES)
      return TOUCH_HOR_RES;
    #if TOUCH_INVERT == 1
        return TOUCH_HOR_RES - result;
    #endif
        return result;
}
/********************************************************************/
static uint16_t touch_get_Y(void)
{
    uint16_t result;
    if(adcY < TOUCH_Y_MIN)
      return 0;

    result = (uint32_t) ((uint32_t) (adcY - TOUCH_Y_MIN) * TOUCH_VER_RES)
    / (TOUCH_Y_MAX - TOUCH_Y_MIN);

    if (result > TOUCH_VER_RES)
      return TOUCH_VER_RES;

    #if TOUCH_INVERT == 1
        return TOUCH_VER_RES - result;
    #endif
        return result;
}

static void setup_x_axis() {
  __HAL_RCC_ADC2_CLK_ENABLE();
  GPIO_InitTypeDef GPIO_InitStruct = { 0 };

  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);
}

static void setup_y_axis() {
  __HAL_RCC_ADC1_CLK_ENABLE();
  GPIO_InitTypeDef GPIO_InitStruct = { 0 };

  GPIO_InitStruct.Pin = GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);
}

