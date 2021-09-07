/*
 * Copyright (c) 2019 Coopermaq
 * All rights reserved
 */
#include "mcp23sxx.h"
#include "hw_output_16.h"
#include "main.h"

#define ADDROUT 0x40  /* HW OUTPUT 16 ADDR */
#define ADDRIN  0x42  /* HW INPUT 16 ADDR */

#define IODIRA  0x00  /* I/O direction A */
#define IODIRB  0x01  /* I/O direction B */
#define IOCON   0x0A  /* I/O config (also 0x0B) */
#define GPPUA   0x0C  /* Pull-up Control port A */
#define GPPUB   0x0D  /* Pull-up Control port B */
#define GPIOPA  0x12  /* port A */
#define GPIOPB  0x13  /* port B */

static uint16_t out_mask = 0x0000;

uint8_t output_16_init(void) {
	uint8_t config[3];

	config[0] = ADDROUT;
	config[1] = IOCON;
	config[2] = 0x0C; /* set ODR and HAEN */
	if (mcp23sxx_write_data(config, 3) != HAL_OK) {
		Error_Handler();
		return HAL_ERROR;
	}

	config[0] = ADDROUT;
	config[1] = IODIRA;
	config[2] = 0x00; /* set all outputs */
	if (mcp23sxx_write_data(config, 3) != HAL_OK) {
		Error_Handler();
		return HAL_ERROR;
	}

	config[0] = ADDROUT;
	config[1] = IODIRB;
	config[2] = 0x00; /* set all outputs */
	if (mcp23sxx_write_data(config, 3) != HAL_OK) {
		Error_Handler();
		return HAL_ERROR;
	}

	config[0] = ADDROUT;
	config[1] = GPIOPA;
	config[2] = (out_mask & 0xFF);

	if (mcp23sxx_write_data(config, 3) != HAL_OK) {
		Error_Handler();
		return HAL_ERROR;
	}

	config[0] = ADDROUT;
	config[1] = GPIOPB;
	config[2] = ((out_mask & 0xFF00) >> 8);

	if (mcp23sxx_write_data(config, 3) != HAL_OK) {
		Error_Handler();
		return HAL_ERROR;
	}

	return HAL_OK;
}

uint8_t input_16_init(void) {
	uint8_t config[3];

	config[0] = ADDRIN;
	config[1] = IOCON;
	config[2] = 0x0C; /* set ODR and HAEN */
	if (mcp23sxx_write_data(config, 3) != HAL_OK) {
		Error_Handler();
		return HAL_ERROR;
	}

	config[0] = ADDRIN;
	config[1] = IODIRA;
	config[2] = 0xFF; /* set all inputs */
	if (mcp23sxx_write_data(config, 3) != HAL_OK) {
		Error_Handler();
		return HAL_ERROR;
	}

	config[0] = ADDRIN;
	config[1] = IODIRB;
	config[2] = 0xFF; /* set all inputs */
	if (mcp23sxx_write_data(config, 3) != HAL_OK) {
		Error_Handler();
		return HAL_ERROR;
	}

	config[0] = ADDRIN;
	config[1] = GPPUA;
	config[2] = 0x00; /* disable pull-up */
	if (mcp23sxx_write_data(config, 3) != HAL_OK) {
		Error_Handler();
		return HAL_ERROR;
	}

	config[0] = ADDRIN;
	config[1] = GPPUB;
	config[2] = 0x00; /* disable pull-up */
	if (mcp23sxx_write_data(config, 3) != HAL_OK) {
		Error_Handler();
		return HAL_ERROR;
	}
	return HAL_OK;
}

uint8_t output_set(uint8_t output) {
	uint8_t config[3] = { };

	if (output > 15) {
		Error_Handler();
		return HAL_ERROR;
	}
	config[0] = ADDROUT;
	config[2] = out_mask;

	if (output < 8) {
		config[1] = GPIOPA;
		config[2] |= (1 << (output));
	} else {
		config[1] = GPIOPB;
		config[2] |= (1 << (output - 8));
	}
	out_mask = config[2];

	if (mcp23sxx_write_data(config, 3) != HAL_OK) {
		Error_Handler();
		return HAL_ERROR;
	}
	return HAL_OK;
}

uint8_t output_reset(uint8_t output) {
	uint8_t config[3] = { };

	if (output > 15) {
		Error_Handler();
		return HAL_ERROR;
	}
	config[0] = ADDROUT;
	config[2] = out_mask;

	if (output < 8) {
		config[1] = GPIOPA;
		config[2] &= ~(1 << (output));
	} else {
		config[1] = GPIOPB;
		config[2] &= ~(1 << (output - 8));
	}
	out_mask = config[2];

	if (mcp23sxx_write_data(config, 3) != HAL_OK) {
		Error_Handler();
		return HAL_ERROR;
	}
	return HAL_OK;
}

uint8_t output_toggle(uint8_t output) {
	uint8_t config[3] = { };
	uint16_t set_port = out_mask;
	if (output > 15) {
		return HAL_ERROR;
	}

	config[0] = ADDROUT;
	set_port ^= (1 << output);
	if (output < 8) {
		config[1] = GPIOPA;
		config[2] = (set_port & 0xFF);
	} else {
		config[1] = GPIOPB;
		config[2] = ((set_port & 0xFF00) >> 8);
	}

	if (set_port == out_mask) {
		return HAL_OK;
	}

	out_mask = set_port;

	if (mcp23sxx_write_data(config, 3) != HAL_OK) {
		return HAL_ERROR;
	}

	return HAL_OK;
}

uint8_t output_read(uint8_t pin, uint8_t* output) {
	uint8_t config[3] = { };

	if (pin > 15) {
		Error_Handler();
		return HAL_ERROR;
	}

	config[0] = ADDROUT | 0x01;

	if (pin < 8) {
		config[1] = GPIOPA;
	} else {
		config[1] = GPIOPB;
		pin = pin - 8;
	}

	if (mcp23sxx_read_data(config, 3) != HAL_OK) {
		Error_Handler();
		return HAL_ERROR;
	}

	*output = (config[2] >> pin) & 0x1;
	return HAL_OK;
}

uint8_t input_read(uint8_t pin, uint8_t* input) {
	uint8_t config[3] = { };

	config[0] = ADDRIN | 0x01;

	if (pin < 8) {
		config[1] = GPIOPA;
	} else {
		config[1] = GPIOPB;
		pin = pin - 8;
	}

	if (mcp23sxx_read_data(config, 3) != HAL_OK) {
		return HAL_ERROR;
	}

	*input = (config[2] >> pin) & 0x01;
//	*input = config[2];

	return HAL_OK;
}
