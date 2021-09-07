/*
 * Copyright (c) 2019 Coopermaq
 * All rights reserved
 */

#include <stdio.h>
#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "mlx90614.h"
#include "util.h"
#include "log.h"

#if MLX90614_FREERTOS == 0
#define mlx90614_delay(x)   HAL_Delay(x)
#else
#include "cmsis_os.h"
#define mlx90614_delay(x)   osDelay(x)
#endif

static HAL_StatusTypeDef MLX90614_WriteReg(uint8_t devAddr, uint8_t regAddr, uint16_t data);
static HAL_StatusTypeDef MLX90614_ReadReg(uint8_t devAddr, uint8_t regAddr, uint16_t *data);

extern I2C_HandleTypeDef hi2c2;
char temp_buff[128] = {};

static HAL_StatusTypeDef MLX90614_WriteReg(uint8_t devAddr, uint8_t regAddr, uint16_t data) {
  uint8_t i2cdata[4], temp[4];
  temp[0] = (devAddr << 1);
  temp[1] = regAddr;
  temp[2] = 0x00;
  temp[3] = 0x00;

  // For a write word command, in the crc8 calculus, you have to include [SA_W, Command, LSB, MSB]
  i2cdata[0] = temp[1]; //EEPROM-address
  i2cdata[1] = temp[2]; //Delete-Byte, low
  i2cdata[2] = temp[3]; //Delete-Byte, high
  i2cdata[3] = crc8(temp, 4);

  if(HAL_I2C_Master_Transmit(&hi2c2, (devAddr << 1), i2cdata, 4, 0xFFFF) != HAL_OK)
    return HAL_ERROR;
  mlx90614_delay(10);

  temp[2] = data & 0xFF; //Getting LSB first
  temp[3] = data >> 8;   //Getting MSB after

  i2cdata[0] = temp[1]; //EEPROM-address
  i2cdata[1] = temp[2]; //Delete-Byte, low
  i2cdata[2] = temp[3]; //Delete-Byte, high
  i2cdata[3] = crc8(temp, 4);

  if(HAL_I2C_Master_Transmit(&hi2c2, (devAddr << 1), i2cdata, 4, 0xFFFF) != HAL_OK)
    return HAL_ERROR;
  mlx90614_delay(10);
  return HAL_OK;
}

static HAL_StatusTypeDef MLX90614_ReadReg(uint8_t devAddr, uint8_t regAddr, uint16_t *data) {
  uint8_t in_buff[3], crc_buff[5], crc;

  *data = 0x0000;
  if(HAL_I2C_Mem_Read(&hi2c2, (devAddr<<1), regAddr, 1, in_buff, 3, 50) != HAL_OK) {
    return HAL_ERROR;
  }

  // For a read word command, in the crc8 calculus, you have to include [SA_W, Command, SA_R, LSB, MSB]
  crc_buff[0] = (devAddr<<1);
  crc_buff[1] = regAddr;
  crc_buff[2] = (devAddr<<1) + 1;
  crc_buff[3] = in_buff[0];
  crc_buff[4] = in_buff[1];
  crc = crc8(crc_buff, 5);

  *data = (in_buff[1] <<8 | in_buff[0]);

  // Implement CRC8 check on data received
  if (crc != in_buff[2]) {
    return HAL_ERROR;
  }

  return HAL_OK;
}

HAL_StatusTypeDef MLX90614_getTempC(uint8_t devAddr, float *temp) {
  uint16_t data;
  if(MLX90614_ReadReg(devAddr, MLX90614_TOBJ1, &data) != HAL_OK) {
    return HAL_ERROR;
  }
  *temp = data*0.02 - 273.15;
  return HAL_OK;
}

HAL_StatusTypeDef MLX90614_getTempF(uint8_t devAddr, float *temp) {
  uint16_t data;
  if(MLX90614_ReadReg(devAddr, MLX90614_TOBJ1, &data) != HAL_OK) {
    return HAL_ERROR;
  }
  *temp = (((data * 0.02) * 1.8) - 459.67) * 1; //to fahrenheit
  return HAL_OK;
}

HAL_StatusTypeDef MLX90614_getAmbientC(uint8_t devAddr, float *temp) {
  uint16_t data;
  if(MLX90614_ReadReg(devAddr, MLX90614_TAMB, &data) != HAL_OK) {
    return HAL_ERROR;
  }
  *temp = data*0.02 - 273.15;
  return HAL_OK;
}

HAL_StatusTypeDef MLX90614_getAmbientF(uint8_t devAddr, float *temp) {
  uint16_t data;
  if(MLX90614_ReadReg(devAddr, MLX90614_TAMB, &data) != HAL_OK) {
    return HAL_ERROR;
  }
  *temp = (((data * 0.02) * 1.8) - 459.67) * 1; //to fahrenheit
  return HAL_OK;
}

HAL_StatusTypeDef MLX90614_getEmissivity(uint8_t devAddr, uint16_t *emiss) {
  return MLX90614_ReadReg(devAddr, MLX90614_EMISSIVITY, emiss);
}

HAL_StatusTypeDef MLX90614_setEmissivity(uint8_t devAddr, uint16_t emiss) {
  return MLX90614_WriteReg(devAddr, MLX90614_EMISSIVITY, emiss);
}

HAL_StatusTypeDef MLX90614_getConfig1(uint8_t devAddr, uint16_t *config) {
  return MLX90614_ReadReg(devAddr, MLX90614_CFG1, config);
}

HAL_StatusTypeDef MLX90614_setConfig1(uint8_t devAddr, uint16_t config) {
  return MLX90614_WriteReg(devAddr, MLX90614_CFG1, config);
}

HAL_StatusTypeDef MLX90614_setDeviceID(uint16_t new_id) {
  return MLX90614_WriteReg(MLX90614_BROADCAST_SA, MLX90614_SA, new_id);
}

uint8_t MLX90614_findDeviceID() {
  uint8_t id = 0x00;
  HAL_StatusTypeDef result;
  for (int i = 1; i<128; i++) {
    result = HAL_I2C_IsDeviceReady(&hi2c2, (uint16_t) (i<<1), 2, 50);
    if (result == HAL_OK) {
      id = i;
      break;
    }
  }
  return id;
}

void MLX90614_ScanDevices () {
  HAL_StatusTypeDef result;
  for (int i = 0; i<128; i++) {
    result = HAL_I2C_IsDeviceReady(&hi2c2, (uint16_t) (i<<1), 2, 50);
    if (result != HAL_OK) {
      sprintf(temp_buff, ".");
      uart6_printf("%s\n\r", temp_buff);
    }
    if (result == HAL_OK) {
      sprintf(temp_buff, "0x%X", i);
      uart6_printf("%s\n\r", temp_buff);
    }
  }
}
