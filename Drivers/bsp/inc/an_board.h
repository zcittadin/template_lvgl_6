/*
 * Copyright (c) 2019 Coopermaq
 * All rights reserved
 */

#ifndef AN_BOARD_H
#define AN_BOARD_H

#include "stm32f4xx_hal.h"

#define ANL_T1      0x00	// Leitura Termistor 1
#define ANL_T2      0x01	// Leitura Termistor 2
#define ANL_UM      0x02	// Leitura Umidade
#define ANL_VT1     0x03	// Leitura Ventilação 1
#define ANL_CO2     0x04	// Leitura CO2
#define ANL_VT2     0x05	// Leitura Ventilação 2 (Antiga RESERVA 1)
#define ANL_BAL     0x06	// Leitura Balança		(Antiga RESERVA 2)
#define ANL_T3      0x07	// Leitura Termistor 3 	(Antiga ANTIGA BAL)

uint8_t an_read_temperature(uint8_t channel, uint16_t *data);
uint8_t an_read_humidity(uint8_t channel , uint16_t *rh_humidity);
uint8_t an_read_damper(uint8_t channel , uint16_t *damper);
uint8_t an_read_co2(uint8_t channel , uint16_t *co2);
uint8_t an_read_res1(uint8_t channel , uint16_t *res1);
uint8_t an_read_weight(uint8_t channel , uint16_t *weight);
uint8_t an_read_res2(uint8_t channel , uint16_t *res2);

#endif /* AN_BOARD_H */
