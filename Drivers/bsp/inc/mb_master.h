/*
 * mb_master.h
 *
 *  Created on: 24 de mar de 2020
 *      Author: Zander Cittadin
 */

#ifndef COOP_INC_MB_MASTER_H_
#define COOP_INC_MB_MASTER_H_

#include "stm32f4xx_hal.h"

#define MB_SLAVE_ADDRESS 0x01
#define MB_READ_HOLDING_REGISTERS 0x03
#define MB_WRITE_SINGLE_REGISTER 0x06
#define MB_REG_TO_READ 0x13
#define MB_PKG_LENGTH 43

uint8_t mb_read_holding_registers(void);
//uint8_t mb_read_holding_registers(uint8_t *tx, uint8_t *rx);
void mb_master_tx_callback(void);
void mb_master_rx_callback(void);

#endif /* COOP_INC_MB_MASTER_H_ */
