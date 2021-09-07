/*
 * Copyright (c) 2019 Coopermaq
 * All rights reserved
 */

#include "ssd1963.h"
#include "lv_drv_conf.h"
#if USE_SSD1963

#include <stdbool.h>
#include LV_DRV_DISP_INCLUDE
#include "stm32f4xx_hal.h"

/*********************
 *      DEFINES
 *********************/
#define SSD1963_CMD_MODE     0
#define SSD1963_DATA_MODE    1

/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *  STATIC PROTOTYPES
 **********************/
static inline void ssd1963_cmd_mode(void);
static inline void ssd1963_data_mode(void);
static inline void ssd1963_cmd(uint16_t cmd);
static inline void ssd1963_data(uint16_t data);
static void ssd1963_io_init(void);
static void ssd1963_reset(void);
static void ssd1963_set_clk(void);
static void ssd1963_set_tft_spec(void);
static void ssd1963_init_bl(void);
void parallel_wr_array(uint16_t * array, uint32_t length);
void parallel_wr_word(uint16_t w);

/**********************
 *  STATIC VARIABLES
 **********************/
static bool cmd_mode = true;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

void ssd1963_init(void) {
  ssd1963_io_init();
  ssd1963_reset();
  ssd1963_set_clk();
  ssd1963_set_tft_spec();
  ssd1963_init_bl();

  ssd1963_cmd(0x13);    //SET display on

  ssd1963_cmd(0x29);    //SET display on
  HAL_Delay(30);
}

void enable_backlight(GPIO_PinState value) {
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, value);
}

void ssd1963_flush(lv_disp_drv_t * drv, const lv_area_t * area, lv_color_t * color_p)
{

    /*Return if the area is out the screen*/
    if(area->x2 < 0) return;
    if(area->y2 < 0) return;
    if(area->x1 > SSD1963_HOR_RES - 1) return;
    if(area->y1 > SSD1963_VER_RES - 1) return;

    /*Truncate the area to the screen*/
    int32_t act_x1 = area->x1 < 0 ? 0 : area->x1;
    int32_t act_y1 = area->y1 < 0 ? 0 : area->y1;
    int32_t act_x2 = area->x2 > SSD1963_HOR_RES - 1 ? SSD1963_HOR_RES - 1 : area->x2;
    int32_t act_y2 = area->y2 > SSD1963_VER_RES - 1 ? SSD1963_VER_RES - 1 : area->y2;

    //Set the rectangular area
    ssd1963_cmd(0x002A);
    ssd1963_data(act_x1 >> 8);
    ssd1963_data(0x00FF & act_x1);
    ssd1963_data(act_x2 >> 8);
    ssd1963_data(0x00FF & act_x2);

    ssd1963_cmd(0x002B);
    ssd1963_data(act_y1 >> 8);
    ssd1963_data(0x00FF & act_y1);
    ssd1963_data(act_y2 >> 8);
    ssd1963_data(0x00FF & act_y2);

    ssd1963_cmd(0x2c);
    int16_t i;
    uint16_t full_w = area->x2 - area->x1 + 1;

    ssd1963_data_mode();
    LV_DRV_DISP_PAR_CS(0);
#if LV_COLOR_DEPTH == 16
    uint16_t act_w = act_x2 - act_x1 + 1;
    for(i = act_y1; i <= act_y2; i++) {
        parallel_wr_array((uint16_t *)color_p, act_w);
        color_p += full_w;
    }
    LV_DRV_DISP_PAR_CS(1);
#else
    int16_t j;
    for(i = act_y1; i <= act_y2; i++) {
        for(j = 0; j <= act_x2 - act_x1 + 1; j++) {
            parallel_wr_word((uint16_t)color_p[j].full);
            color_p += full_w;
        }
    }
#endif

    lv_disp_flush_ready(drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

static void ssd1963_io_init(void) {
  LV_DRV_DISP_CMD_DATA(SSD1963_CMD_MODE);
  cmd_mode = true;
}

/**
 * Write word to the parallel port
 * @param w a word to write
 */
void parallel_wr_word(uint16_t w) {

  RESET_CS;
  SET_RD;
  SET_WR;
  TFT_DataPort = (w & 0xFFFF);
  WR_STROBE
  SET_CS;
}

/**
 * Write an array to the parallel port
 * @param array pointer to a memory block to write
 * @param length number of words (not bytes) to write
 */
void parallel_wr_array(uint16_t * array, uint32_t length) {
  uint32_t i;

  for(i = 0; i < length; i++) {
    GPIOE->ODR = *array; //Set data and write pin ns delay
    GPIOC->ODR &= ~(1 << 9);
    array++; //Buffer increase and write pin ns delay
    GPIOC->ODR |= (1 << 9);
  }
}


static void ssd1963_reset(void) {
  /*Hardware reset*/
  LV_DRV_DISP_RST(1);
  HAL_Delay(50);
  LV_DRV_DISP_RST(0);
  HAL_Delay(50);
  LV_DRV_DISP_RST(1);
  HAL_Delay(50);

  /*Chip enable*/
  LV_DRV_DISP_PAR_CS(0);
  HAL_Delay(10);
  LV_DRV_DISP_PAR_CS(1);
  HAL_Delay(5);

  /*Software reset*/
  ssd1963_cmd(0x01);
  HAL_Delay(20);

  ssd1963_cmd(0x01);
  HAL_Delay(20);

  ssd1963_cmd(0x01);
  HAL_Delay(20);

}

static void ssd1963_set_clk(void) {
  /* Set PLL*/
  ssd1963_cmd(0xe2);
  ssd1963_data(0x1d);
  ssd1963_data(0x02);
  ssd1963_data(0x54);

  /*Enable PLL*/
  ssd1963_cmd(0xe0);
  ssd1963_data(0x01);
  HAL_Delay(20);

  /*Lock PLL*/
  ssd1963_cmd(0xe0);
  ssd1963_data(0x03);

  /*Software reset*/
  ssd1963_cmd(0x01);
  HAL_Delay(20);

  /*Set PCLK freq*/
  ssd1963_cmd(0xe6);
  ssd1963_data(0x04);
  ssd1963_data(0xd7);
  ssd1963_data(0xf9);
}

static void ssd1963_set_tft_spec(void) {
  ssd1963_cmd(0xB0); //LCD SPECIFICATION
  ssd1963_data(0x10);
  ssd1963_data(0x80);
  ssd1963_data(0x03);
  ssd1963_data(0x1F);
  ssd1963_data(0x01);
  ssd1963_data(0xDF);
  ssd1963_data(0x00);

  ssd1963_cmd(0xB4); //HSYNC
  ssd1963_data(0x04);
  ssd1963_data(0x20);
  ssd1963_data(0x00);
  ssd1963_data(0x2E);
  ssd1963_data(0xD2);
  ssd1963_data(0x00);
  ssd1963_data(0x00);
  ssd1963_data(0x00);

  ssd1963_cmd(0xB6); //VSYNC
  ssd1963_data(0x02);
  ssd1963_data(0x0D);
  ssd1963_data(0x00);
  ssd1963_data(0x17);
  ssd1963_data(0x16);
  ssd1963_data(0x00);
  ssd1963_data(0x00);

  ssd1963_cmd(0x2A);
  ssd1963_data(0x00);
  ssd1963_data(0x00);
  ssd1963_data(0x03);
  ssd1963_data(0x1F);

  ssd1963_cmd(0x2B);
  ssd1963_data(0x00);
  ssd1963_data(0x00);
  ssd1963_data(0x01);
  ssd1963_data(0xDF);

  ssd1963_cmd(0xf0); //SET pixel data I/F format=16bit
  ssd1963_data(0x03);

  ssd1963_cmd(0x36); //Orientação 180°
#if SSD1963_ORI == 1
  ssd1963_data(0x02);
#else
  ssd1963_data(0x01);
#endif
}

static void ssd1963_init_bl(void) {

  ssd1963_cmd(0xBE); /*Set PWM configuration for back light control*/

  ssd1963_data(0x02); /*PWMF[7:0] = 2, PWM base freq = PLL/(256*(1+5))/256 =
                        300Hz for a PLL freq = 120MHz */
  ssd1963_data(0x20); /*Set duty cycle, from 0x00 (total pull-down) to 0xFF
                        (99% pull-up , 255/256) */
  ssd1963_data(0x01); /*PWM enabled and controlled by host (mcu) */
  ssd1963_data(0x00);
  ssd1963_data(0x00);
  ssd1963_data(0x00);
}

/**
 * Command mode
 */
static inline void ssd1963_cmd_mode(void) {
  if (cmd_mode == false) {
    LV_DRV_DISP_CMD_DATA(SSD1963_CMD_MODE);
    cmd_mode = true;
  }
}

/**
 * Data mode
 */
static inline void ssd1963_data_mode(void) {
  if (cmd_mode != false) {
    LV_DRV_DISP_CMD_DATA(SSD1963_DATA_MODE);
    cmd_mode = false;
  }
}

/**
 * Write command
 * @param cmd the command
 */
static inline void ssd1963_cmd(uint16_t cmd) {
  ssd1963_cmd_mode();
  parallel_wr_word(cmd);
}

/**
 * Write data
 * @param data the data
 */
static inline void ssd1963_data(uint16_t data) {
  ssd1963_data_mode();
  parallel_wr_word(data);
}

#endif
