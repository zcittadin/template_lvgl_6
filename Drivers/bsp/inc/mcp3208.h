/*
 * Copyright (c) 2019 Coopermaq
 * All rights reserved
 */

#ifndef MCP3208_H
#define MCP3208_H

#include "stm32f4xx_hal.h"

uint8_t mcp3208_read_channel(uint8_t channel, uint16_t *data);

#endif /* MCP3208_H */
