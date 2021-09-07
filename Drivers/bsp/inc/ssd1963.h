/*
 * Copyright (c) 2019 Coopermaq
 * All rights reserved
 */

#ifndef SSD1963_H
#define SSD1963_H

/*********************
 *      INCLUDES
 *********************/
#include "lv_drv_conf.h"
#if USE_SSD1963 != 0

#include <stdint.h>
#include "lv_color.h"
//#include "lv_color.h"
#include "lv_disp.h"
//#include "lv_hal_disp.h"

/*********************
 *      DEFINES
 *********************/

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 * GLOBAL PROTOTYPES
 **********************/
void ssd1963_init(void);
void enable_backlight(GPIO_PinState value);
void ssd1963_flush(lv_disp_drv_t * drv, const lv_area_t * area, lv_color_t * color_p);

/**********************
 *      MACROS
 **********************/
#define NOP asm("nop");
#define TFT_DataPort GPIOE->ODR
#define SET_CS GPIOD->ODR |= (1 << 3)
#define RESET_CS GPIOD->ODR &= ~(1 << 3)
#define SET_RST GPIOD->ODR |= (1 << 11)
#define RESET_RST GPIOD->ODR &= ~(1 << 11)
#define SET_RS GPIOD->ODR |= (1 << 12)
#define RESET_RS GPIOD->ODR &= ~(1 << 12)
#define SET_WR GPIOC->ODR |= (1 << 9)
#define RESET_WR GPIOC->ODR &= ~(1 << 9)
#define SET_RD GPIOC->ODR |= (1 << 8)
#define RESET_RD GPIOC->ODR &= ~(1 << 8)
#define WR_STROBE  RESET_WR; NOP; SET_WR;

#endif

#endif
