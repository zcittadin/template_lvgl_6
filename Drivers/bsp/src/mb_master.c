/*
 * mb_master.c
 *
 *  Created on: 24 de mar de 2020
 *      Author: Zander Cittadin
 */

#include "mb_master.h"
#include "cmsis_os.h"
#include "usart.h"
#include "util.h"
#include "log.h"

//static uint8_t mb_master_transaction(uint8_t *tx, uint8_t *rx);

__IO ITStatus uart2_ready = RESET;

static uint8_t mb_master_transaction(uint8_t *tx, uint8_t *rx, size_t rx_size) {
	uint32_t tickstart;
	memset(rx, 0, MB_PKG_LENGTH);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_SET);

	uart2_ready = RESET;
	if (HAL_UART_Transmit_DMA(&huart2, tx, 8) != HAL_OK) {
		return HAL_ERROR;
	}

	tickstart = HAL_GetTick();
	while ((uart2_ready != SET) && ((HAL_GetTick() - tickstart) <= 30)) {
	}

	uart2_ready = RESET;
	if (HAL_UART_Receive_DMA(&huart2, rx, MB_PKG_LENGTH) != HAL_OK) {
		return HAL_ERROR;
	}

	tickstart = HAL_GetTick();
	while ((uart2_ready != SET) && ((HAL_GetTick() - tickstart) <= 100)) {
		osDelay(5);
	}

	return HAL_OK;
}

uint8_t mb_read_holding_registers() {
	//Prepara pacote de solicitação
	uint8_t arr_crc[2];
	uint8_t mb_rx[MB_PKG_LENGTH];
	uint8_t pre_tx[] = { MB_SLAVE_ADDRESS, MB_READ_HOLDING_REGISTERS, 0x00,
			0x00, 0x00, MB_REG_TO_READ };
	uint16_t _crc = crc16(pre_tx, 6);
	word_to_byte(_crc, arr_crc);
	uint8_t rs_tx[] = { MB_SLAVE_ADDRESS, MB_READ_HOLDING_REGISTERS, 0x00, 0x00,
			0x00, MB_REG_TO_READ, arr_crc[0], arr_crc[1] };
	size_t size = sizeof(mb_rx) / sizeof(uint8_t);

	//Envia solicitação
	mb_master_transaction(rs_tx, mb_rx, size);

	//Compara ID e código de função enviada/recebida
	if ((mb_rx[0] == rs_tx[0]) && (mb_rx[1] == rs_tx[1])) {
		HAL_UART_Abort(&huart2);
		return HAL_OK;
	}
	HAL_UART_Abort(&huart2);
	return HAL_ERROR;
}

void mb_master_tx_callback() {
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_RESET);
	uart2_ready = SET;
}

void mb_master_rx_callback() {
	uart2_ready = SET;
}
