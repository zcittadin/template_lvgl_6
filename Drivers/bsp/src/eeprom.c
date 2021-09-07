/*
 * Copyright (c) 2021 Coopermaq
 * All rights reserved
 * Author: Zander Cittadin
 */

#include "cmsis_os.h"
#include "stm32f4xx_hal.h"
#include "i2c.h"
#include "eeprom.h"
#include "log.h"

#define EEPROM_ADDR (0x50 << 1)
SemaphoreHandle_t eeprom_handle = NULL;

uint8_t eeprom_init(void) {
  eeprom_handle = xSemaphoreCreateMutex();
  return HAL_OK;
}

uint8_t eeprom_write(uint16_t mem_addr, uint8_t *msg, uint8_t size) {
  uint32_t tickstart;
  HAL_StatusTypeDef status = HAL_BUSY;

  tickstart = HAL_GetTick();
  while (status != HAL_OK) {
    if ((HAL_GetTick() - tickstart) >= 1000) {
      return status;
    }

    status = HAL_I2C_Mem_Write(&hi2c1, EEPROM_ADDR, mem_addr, I2C_MEMADD_SIZE_16BIT,
        (uint8_t*)msg, size, 200);
    osDelay(30);
  }

  return status;
}

uint8_t eeprom_read(uint16_t mem_addr, uint8_t *msg, uint8_t size) {
  HAL_StatusTypeDef status = HAL_BUSY;
  uint32_t tickstart;

  tickstart = HAL_GetTick();
  while (status != HAL_OK) {
    if ((HAL_GetTick() - tickstart) >= 1000) {
      return status;
    }

    status = HAL_I2C_IsDeviceReady(&hi2c1, EEPROM_ADDR, 1, 20);
    osDelay(30);
  }

  status = HAL_BUSY;
  tickstart = HAL_GetTick();
  while (status != HAL_OK) {
    if ((HAL_GetTick() - tickstart) >= 200) {
      return status;
    }

    status = HAL_I2C_Mem_Read(&hi2c1, EEPROM_ADDR, mem_addr, I2C_MEMADD_SIZE_16BIT,
        (uint8_t*)msg, size, 200);
    osDelay(30);
  }
  return HAL_OK;
}

uint8_t eeprom_save_data(uint16_t addr, void *data, uint16_t size) {
  uint8_t position = 0;
  if (xSemaphoreTake(eeprom_handle, portMAX_DELAY) != pdTRUE) {
    return HAL_ERROR;
  }

  while (size > EEPROM_PAGE_SIZE) {
    size -= EEPROM_PAGE_SIZE;
    if (eeprom_write(addr+(position*EEPROM_PAGE_SIZE), (uint8_t *)data+(position*EEPROM_PAGE_SIZE),\
          EEPROM_PAGE_SIZE) != HAL_OK) {
      xSemaphoreGive(eeprom_handle);
      return HAL_ERROR;
    }
    position++;
  }
  if (eeprom_write(addr+(position*EEPROM_PAGE_SIZE), (uint8_t *)data+(position*EEPROM_PAGE_SIZE),\
        (uint8_t)size) != HAL_OK) {
    xSemaphoreGive(eeprom_handle);
    return HAL_ERROR;
  }

  xSemaphoreGive(eeprom_handle);
  return HAL_OK;
}

uint8_t eeprom_load_data(uint16_t addr, void *data, uint16_t size) {
  uint8_t position = 0;
  if (xSemaphoreTake(eeprom_handle, portMAX_DELAY) != pdTRUE) {
    return HAL_ERROR;
  }

  while (size > EEPROM_PAGE_SIZE) {
    size -= EEPROM_PAGE_SIZE;
    if (eeprom_read(addr+(position*EEPROM_PAGE_SIZE),(uint8_t *)data+(position*EEPROM_PAGE_SIZE),\
          EEPROM_PAGE_SIZE) != HAL_OK) {
      xSemaphoreGive(eeprom_handle);
      return HAL_ERROR;
    }
    position++;
  }
  if (eeprom_read(addr+(position*EEPROM_PAGE_SIZE), (uint8_t *)data+(position*EEPROM_PAGE_SIZE),\
        size) != HAL_OK) {
    xSemaphoreGive(eeprom_handle);
    return HAL_ERROR;
  }
  xSemaphoreGive(eeprom_handle);
  return HAL_OK;
}

