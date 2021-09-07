/*
 * Copyright (c) 2019 Coopermaq
 * All rights reserved
 */

#ifndef BUZZER_H
#define BUZZER_H

#include "stm32f4xx_hal.h"

/* BEEP PADRÃO */
#define STD_BEEP 80

uint8_t buzzer(uint32_t time_ms);

#endif /* BUZZER_H */
