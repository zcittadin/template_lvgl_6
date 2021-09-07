/*
 * Copyright (c) 2019 Coopermaq
 * All rights reserved
 */

#include "mcp3208.h"
#include "spi.h"
#include "gpio.h"
#include "dwt_stm32_delay.h"

#define SPI_TIMEOUT 50 /* timeout ms */

uint8_t mcp3208_read_channel(uint8_t channel, uint16_t *data) {
  uint8_t transmit[3] = { 0 };
  uint8_t receive [3] = { 0xFF, 0xFF, 0xFF };
  if (channel > 3)
    transmit[0] = 0x07;
  else
    transmit[0] = 0x06;

  transmit[1] = channel << 6;

  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
  DWT_Delay_us(100);

  if (HAL_SPI_TransmitReceive(&hspi2, transmit, receive, 3, SPI_TIMEOUT) != HAL_OK) {
    Error_Handler();
    return HAL_ERROR;
  }

  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_SET);

  *data = ((receive[1] & 0x0F) << 8) | receive[2]; // 12 BIT ADC Wert zusammenfuegen
  return HAL_OK;
}

