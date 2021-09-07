/*
 * Copyright (c) 2020 Coopermaq
 * All rights reserved
 * Author: Zander Cittadin
 *
 * Based on driver found at: https://github.com/andrey-gvrd/m25p80
 *
 */

#ifndef M25P80_H
#define M25P80_H

#include <stdbool.h>  // bool
#include <stddef.h>   // size_t
#include <stdint.h>   // uint_t
#include "stm32f4xx_hal.h"

#define M25P80_PAGE_SIZE      256  // bytes
#define M25P80_SECTOR_SIZE    256  // pages
#define M25P80_SECTOR_NUMBER  16
#define M25P80_PAGE_NUMBER    (M25P80_SECTOR_SIZE * M25P80_SECTOR_NUMBER)

#define FLASH_SET_CS             HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_SET);
#define FLASH_RESET_CS           HAL_GPIO_WritePin(GPIOD, GPIO_PIN_7, GPIO_PIN_RESET);

typedef struct {
  uint8_t
    srwd:1,    // b7
    unused1:1,
    unused2:1,
    bp2:1,
    bp1:1,
    bp0:1,
    wel:1,
    wip:1;     // b0
} flash_status_t;

void flash_init(void);

bool flash_verify_signature(void);
bool flash_is_write_busy(void);
void flash_wakeup(void);
void flash_power_down(void);
bool flash_erase_bulk(void);
void flash_write(uint8_t *buffer, size_t buffer_len, uint32_t pageAddress);
void flash_read (uint8_t *buffer, size_t buffer_len, uint32_t pageAddress);

#define flash_save_page(page, buffer) flash_write((buffer), ((uint32_t)(page)) << 8, M25P80_PAGE_SIZE)
#define flash_load_page(page, buffer) flash_read(((uint32_t)(page)) << 8, (buffer), M25P80_PAGE_SIZE)

#endif // M25P80_H