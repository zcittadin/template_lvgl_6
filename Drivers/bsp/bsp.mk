CSRCS += src/an_board.c
CSRCS += src/buzzer.c
CSRCS += src/eeprom.c
CSRCS += src/dwt_stm32_delay.c
CSRCS += src/ftoa.c
CSRCS += src/hal_smbus.c
CSRCS += src/hw_output_16.c
CSRCS += src/log.c
CSRCS += src/m25p80.c
CSRCS += src/mb_master.c
CSRCS += src/mcp23sxx.c
CSRCS += src/mcp3208.c
CSRCS += src/mlx90614.c
CSRCS += src/rtc_util.c
CSRCS += src/ssd1963.c
CSRCS += src/touchscreen.c
CSRCS += src/util.c

DEPPATH += --dep-path Drivers/bsp/src
VPATH += :Drivers/bsp/src

CFLAGS += -IDrivers/bsp/inc
