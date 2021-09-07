/*
 * Copyright (c) 2019 Coopermaq
 * All rights reserved
 */

#ifndef MCP23SXX_H
#define MCP23SXX_H

#include "stm32f4xx_hal.h"

uint8_t mcp23sxx_write_data(uint8_t *data, uint8_t len);
uint8_t mcp23sxx_read_data(uint8_t *data, uint8_t len);

#endif /* MCP23SXX_H */
