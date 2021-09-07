/*
 * Copyright (c) 2019 Coopermaq
 * All rights reserved
 */

#ifndef HW_OUTPUT_16_H
#define HW_OUTPUT_16_H

#include "stm32f4xx_hal.h"

uint8_t output_16_init(void);
uint8_t input_16_init(void);
uint8_t output_set(uint8_t output);
uint8_t output_reset(uint8_t output);
uint8_t output_toggle(uint8_t output);
uint8_t output_read(uint8_t pin, uint8_t* output);
uint8_t input_read(uint8_t pin, uint8_t* input);

#endif /* HW_OUTPUT_16_H */
