/*
 * Copyright (c) 2019 Coopermaq
 * All rights reserved
 */

#include "mcp23sxx.h"
#include "spi.h"
#include "gpio.h"

#define SPI_TIMEOUT 50 /* timeout ms */

uint8_t mcp23sxx_write_data(uint8_t *data, uint8_t len) {
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
  if (HAL_SPI_Transmit(&hspi2, data, len, SPI_TIMEOUT) != HAL_OK) {
    Error_Handler();
    return HAL_ERROR;
  }
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
  return HAL_OK;
}

uint8_t mcp23sxx_read_data(uint8_t *data, uint8_t len) {
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
  if (HAL_SPI_Transmit(&hspi2, data, 2, SPI_TIMEOUT) != HAL_OK) {
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
    Error_Handler();
    return HAL_ERROR;
  }

  if (HAL_SPI_Receive(&hspi2, &data[2], 1, SPI_TIMEOUT) != HAL_OK) {
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
    Error_Handler();
    return HAL_ERROR;
  }
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);

  return HAL_OK;
}

