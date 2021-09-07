/*
 * Copyright (c) 2019 Coopermaq
 * All rights reserved
 */

#ifndef SMBUS_H
#define SMBUS_H

#include "stm32f4xx_hal.h"

HAL_StatusTypeDef HAL_I2C_SMbus_Data(I2C_HandleTypeDef *hi2c, uint16_t DevAddress, uint8_t *command, uint8_t *rxData, uint16_t rxSize, uint32_t Timeout);

#endif /* SMBUS_H */
