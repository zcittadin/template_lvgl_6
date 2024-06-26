/**
 * @file lv_drv_conf.h
 * 
 */

#ifndef LV_DRV_CONF_H
#define LV_DRV_CONF_H

#include "lv_conf.h"
#include "dwt_stm32_delay.h"

/*********************
 * DELAY INTERFACE
 *********************/
#define LV_DRV_DELAY_INCLUDE  <stdint.h>            /*Dummy include by default*/
#define LV_DRV_DELAY_US(us)  /*delay_us(us)*/       /*Delay the given number of microseconds*/
#define LV_DRV_DELAY_MS(ms)  /*delay_ms(ms)*/       /*Delay the given number of milliseconds*/

/*********************
 * DISPLAY INTERFACE
 *********************/

/*------------
 *  Common
 *------------*/
#define LV_DRV_DISP_INCLUDE        "ssd1963.h"           /*Dummy include by default*/
#define LV_DRV_DISP_CMD_DATA(val)  ((val == 1) ? (SET_RS) : (RESET_RS));    /*Set the command/data pin to 'val'*/
#define LV_DRV_DISP_RST(val)       ((val == 1) ? (SET_RST) : (RESET_RST))    /*Set the reset pin to 'val'*/

/*---------
 *  SPI
 *---------*/
#define LV_DRV_DISP_SPI_CS(val)          /*spi_cs_set(val)*/     /*Set the SPI's Chip select to 'val'*/
#define LV_DRV_DISP_SPI_WR_BYTE(data)    /*spi_wr(data)*/        /*Write a byte the SPI bus*/
#define LV_DRV_DISP_SPI_WR_ARRAY(adr, n) /*spi_wr_mem(adr, n)*/  /*Write 'n' bytes to SPI bus from 'adr'*/

/*------------------
 *  Parallel port
 *-----------------*/
#define LV_DRV_DISP_PAR_CS(val)          ((val == 1) ? (SET_CS) : (RESET_CS))   /*Set the Parallel port's Chip select to 'val'*/
#define LV_DRV_DISP_PAR_SLOW             /*par_slow()*/        /*Set low speed on the parallel port*/
#define LV_DRV_DISP_PAR_FAST             /*par_fast()*/        /*Set high speed on the parallel port*/
#define LV_DRV_DISP_PAR_WR_WORD(data)    /*par_wr(data)*/      /*Write a word to the parallel port*/
#define LV_DRV_DISP_PAR_WR_ARRAY(adr, n) /*par_wr_mem(adr,n)*/ /*Write 'n' bytes to Parallel ports from 'adr'*/

/***************************
 * INPUT DEVICE INTERFACE
 ***************************/

/*----------
 *  Common
 *----------*/
#define LV_DRV_INDEV_INCLUDE     "my_touch.h"            /*Dummy include by default*/
#define LV_DRV_INDEV_RST(val)    /*pin_x_set(val)*/     /*Set the reset pin to 'val'*/
#define LV_DRV_INDEV_IRQ_READ    /*pn_x_read()*/      /*Read the IRQ pin*/

/*---------
 *  SPI
 *---------*/
#define LV_DRV_INDEV_SPI_CS(val)			/*Set the SPI's Chip select to 'val'*/
#define LV_DRV_INDEV_SPI_XCHG_BYTE(data)    /*Write 'val' to SPI and give the read value*/

/*---------
 *  I2C
 *---------*/
#define LV_DRV_INDEV_I2C_START              /*i2c_start()*/       /*Make an I2C start*/
#define LV_DRV_INDEV_I2C_STOP               /*i2c_stop()*/        /*Make an I2C stop*/
#define LV_DRV_INDEV_I2C_RESTART            /*i2c_restart()*/     /*Make an I2C restart*/
#define LV_DRV_INDEV_I2C_WR(data)           /*i2c_wr(data)*/      /*Write a byte to the I1C bus*/
#define LV_DRV_INDEV_I2C_READ(last_read)    0 /*i2c_rd()*/        /*Read a byte from the I2C bud*/


/*********************
 *  DISPLAY DRIVERS
 *********************/

/*-------------------
 *  Monitor of PC
 *-------------------*/
#define USE_MONITOR         0
#if USE_MONITOR
#define MONITOR_HOR_RES     LV_HOR_RES_MAX
#define MONITOR_VER_RES     LV_HOR_VER
#endif

/*----------------
 *    SSD1963
 *--------------*/
#define USE_SSD1963         1
#if USE_SSD1963
#define SSD1963_HOR_RES     LV_HOR_RES_MAX
#define SSD1963_VER_RES     LV_VER_RES_MAX
#define SSD1963_HDP         799
#define SSD1963_HT          900
#define SSD1963_HPS         50
#define SSD1963_LPS         10
#define SSD1963_HPW         10
#define SSD1963_VDP         479
#define SSD1963_VT          520
#define SSD1963_VPS         12
#define SSD1963_FPS         4
#define SSD1963_VPW         10
#define SSD1963_HS_NEG      0   /*Negative hsync*/
#define SSD1963_VS_NEG      0   /*Negative vsync*/
#define SSD1963_ORI         0   /*1 -> 180o 0 -> 0o */
#define SSD1963_COLOR_DEPTH 16
#endif

/*----------------
 *    R61581
 *--------------*/
#define USE_R61581          0
#if USE_R61581 != 0
#define R61581_HOR_RES      LV_HOR_RES_MAX
#define R61581_VER_RES      LV_VER_RES_MAX
#define R61581_HDP          479
#define R61581_HT           531
#define R61581_HPS          43
#define R61581_LPS          8
#define R61581_HPW          10
#define R61581_VDP          271
#define R61581_VT           319
#define R61581_VPS          12
#define R61581_FPS          4
#define R61581_VPW          10
#define R61581_HS_NEG       0       /*Negative hsync*/
#define R61581_VS_NEG       0       /*Negative vsync*/
#define R61581_ORI          180     /*0, 90, 180, 270*/
#define R61581_LV_COLOR_DEPTH 16
#endif

/*------------------------------
 *  ST7565 (Monochrome, low res.)
 *-----------------------------*/
#define USE_ST7565          0
#if USE_ST7565 != 0
/*No settings*/
#endif  /*USE_ST7565*/

/*-----------------------------------------
 *  Linux frame buffer device (/dev/fbx)
 *-----------------------------------------*/
#define USE_FBDEV           0
#if USE_FBDEV != 0
#define FBDEV_PATH          "/dev/fb0"
#endif

/*====================
 * Input devices
 *===================*/

/*--------------
 *    XPT2046
 *--------------*/
#define USE_XPT2046         0
//#define XPT2046_XY_SWAP  1
//#define XPT2046_X_INV   1
//#define XPT2046_Y_INV   1
#if USE_XPT2046 != 0
#define XPT2046_HOR_RES     800
#define XPT2046_VER_RES     480
#define XPT2046_X_MIN       75
#define XPT2046_Y_MIN       45
#define XPT2046_X_MAX       3850
#define XPT2046_Y_MAX       3850
#define XPT2046_AVG         1
#define XPT2046_INV         0 
#endif

/*-----------------
 *    FT5406EE8
 *-----------------*/
#define USE_FT5406EE8       0
#if USE_FT5406EE8
#define FT5406EE8_I2C_ADR   0x38                  /*7 bit address*/
#endif

/*-------------------------------
 *    Mouse or touchpad on PC
 *------------------------------*/
#define USE_MOUSE           0
#if USE_MOUSE
/*No settings*/
#endif


/*-------------------------------
 *   Keyboard of a PC
 *------------------------------*/
#define USE_KEYBOARD        0
#if USE_KEYBOARD
/*No settings*/
#endif

#endif  /*LV_DRV_CONF_H*/
