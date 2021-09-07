/*
 * Copyright (c) 2020 Coopermaq
 * All rights reserved
 */

#ifndef MLX90614_H_
#define MLX90614_H_

#include "stm32f4xx_hal.h"

/* Private defines -----------------------------------------------------------*/
/* DEFAULT SLAVE ADDRESS */
#define MLX90614_BROADCAST_SA 0x00
#define MLX90614_DEFAULT_SA 0x01
/* OPCODE DEFINES */
#define MLX90614_OP_RAM		0x00
#define MLX90614_OP_EEPROM	0x20
#define MLX90614_OP_SLEEP	0xFF

/* Sensor module configuration */
#define MLX90614_CONFIG_1 	0x9FB4
#define MLX90614_CONFIG_2 	0xAFF0
#define MLX90614_CONFIG_3 	0x158C
#define MLX90614_CONFIG_4 	0xAFF0

/* RAM offsets with 16-bit data, MSB first */
#define MLX90614_RAW1		(MLX90614_OP_RAM | 0x04) /* raw data IR channel 1 */
#define MLX90614_RAW2		(MLX90614_OP_RAM | 0x05) /* raw data IR channel 2 */
#define MLX90614_TAMB 		(MLX90614_OP_RAM | 0x06) /* ambient temperature */
#define MLX90614_TOBJ1 		(MLX90614_OP_RAM | 0x07) /* object 1 temperature */
#define MLX90614_TOBJ2 		(MLX90614_OP_RAM | 0x08) /* object 2 temperature */
/* EEPROM offsets with 16-bit data, MSB first */
#define MLX90614_TOMIN 		(MLX90614_OP_EEPROM | 0x00) /* object temperature min register */
#define MLX90614_TOMAX 		(MLX90614_OP_EEPROM | 0x01) /* object temperature max register */
#define MLX90614_PWMCTRL 	(MLX90614_OP_EEPROM | 0x02) /* pwm configuration register */
#define MLX90614_TARANGE 	(MLX90614_OP_EEPROM | 0x03) /* ambient temperature register */
#define MLX90614_EMISSIVITY (MLX90614_OP_EEPROM | 0x04) /* emissivity correction register */
#define MLX90614_CFG1 		(MLX90614_OP_EEPROM | 0x05) /* configuration register */
#define MLX90614_SA 		(MLX90614_OP_EEPROM | 0x0E) /* slave address register */
#define MLX90614_ID1 		(MLX90614_OP_EEPROM | 0x1C) /*[read-only] 1 ID register */
#define MLX90614_ID2 		(MLX90614_OP_EEPROM | 0x1D) /*[read-only] 2 ID register */
#define MLX90614_ID3 		(MLX90614_OP_EEPROM | 0x1E) /*[read-only] 3 ID register */
#define MLX90614_ID4 		(MLX90614_OP_EEPROM | 0x1F) /*[read-only] 4 ID register */

/*	Using FreeRTOS 	*/
#define MLX90614_FREERTOS	0

/* Exported functions prototypes ---------------------------------------------*/
HAL_StatusTypeDef MLX90614_getTempC(uint8_t devAddr, float *temp);
HAL_StatusTypeDef MLX90614_getTempF(uint8_t devAddr, float *temp);
HAL_StatusTypeDef MLX90614_getAmbientC(uint8_t devAddr, float *temp);
HAL_StatusTypeDef MLX90614_getAmbientF(uint8_t devAddr, float *temp);
HAL_StatusTypeDef MLX90614_getEmissivity(uint8_t devAddr, uint16_t *emiss);
HAL_StatusTypeDef MLX90614_setEmissivity(uint8_t devAddr, uint16_t emiss);
HAL_StatusTypeDef MLX90614_getConfig1(uint8_t devAddr, uint16_t *config);
HAL_StatusTypeDef MLX90614_setConfig1(uint8_t devAddr, uint16_t config);
HAL_StatusTypeDef MLX90614_setDeviceID(uint16_t new_id);
uint8_t MLX90614_findDeviceID(void);
void MLX90614_ScanDevices(void);

#endif /* MLX90614_H_ */