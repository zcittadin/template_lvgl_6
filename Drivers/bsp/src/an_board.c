/*
 * Copyright (c) 2019 Coopermaq
 * All rights reserved
 */
#include <math.h>

#include "mcp3208.h"
#include "an_board.h"
#include "main.h"

#include "log.h"

uint8_t an_read_temperature(uint8_t channel, uint16_t *data) {
  uint16_t adc;
  float temp;

  if (mcp3208_read_channel(channel, &adc) != HAL_OK) {
    Error_Handler();
    return HAL_ERROR;
  }

  temp = (-(pow((adc), 2.) * 0.00002) + (0.2859 * (adc)) + 246);
  temp = temp - ((0.0001 * (pow(temp, 2.))) - (0.1291 * temp) + 46.326);

  *data = (uint16_t)(temp);
  return HAL_OK;
}

uint8_t an_read_humidity(uint8_t channel , uint16_t *rh_humidity) {
  float temp;
  uint16_t adc;

  if (mcp3208_read_channel(channel, &adc) != HAL_OK) {
    Error_Handler();
    return HAL_ERROR;
  }
  temp = adc * 10;
  temp -= 5000.0;
  temp /= 30;

  *rh_humidity = (uint16_t)temp;
  return HAL_OK;
}

uint8_t an_read_damper(uint8_t channel , uint16_t *damper) {
  uint16_t adc = 0;

  if (mcp3208_read_channel(channel, &adc) != HAL_OK) {
    Error_Handler();
    return HAL_ERROR;
  }

  *damper = adc / 39;
  return HAL_OK;
}

uint8_t an_read_co2(uint8_t channel , uint16_t *co2) {
  float temp;
  uint16_t adc = 0;

  if (mcp3208_read_channel(channel, &adc) != HAL_OK) {
    Error_Handler();
    return HAL_ERROR;
  }

  temp = (float)adc;
  temp -= 755.0;
  temp /= 3296.0;
  temp *= 1000.0;
  *co2 = (uint16_t)temp;
  return HAL_OK;
}

uint8_t an_read_res1(uint8_t channel , uint16_t *res1) {
  uint16_t adc = 0;

  if (mcp3208_read_channel(channel , &adc) != HAL_OK) {
    Error_Handler();
    return HAL_ERROR;
  }

  *res1 = adc;
  return HAL_OK;
}

uint8_t an_read_weight(uint8_t channel , uint16_t *weight) {
  uint16_t adc = 0;

  if (mcp3208_read_channel(channel , &adc) != HAL_OK) {
    Error_Handler();
    return HAL_ERROR;
  }

  *weight = adc;
  return HAL_OK;
}

uint8_t an_read_res2(uint8_t channel , uint16_t *res2) {
  uint16_t adc = 0;

  if (mcp3208_read_channel(channel, &adc) != HAL_OK) {
    Error_Handler();
    return HAL_ERROR;
  }

  *res2 = adc;
  return HAL_OK;
}

