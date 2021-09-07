/*
 * Copyright (c) 2021 Coopermaq
 * All rights reserved
 * Author: Zander Cittadin
 */

#ifndef EEPROM_H
#define EEPROM_H

#include "stm32f4xx_hal.h"

#define EEPROM_PAGE_SIZE            128

uint8_t eeprom_init(void);
uint8_t eeprom_write(uint16_t mem_addr, uint8_t *msg, uint8_t size);
uint8_t eeprom_read(uint16_t mem_addr, uint8_t *msg, uint8_t size);
uint8_t eeprom_save_data(uint16_t addr, void *data, uint16_t size);
uint8_t eeprom_load_data(uint16_t addr, void *data, uint16_t size);

#endif /* EEPROM_H */
