/*
 * Copyright (c) 2021 Coopermaq
 * All rights reserved
 * Author: Zander Cittadin
 *
 * Based on driver found at: https://github.com/andrey-gvrd/m25p80
 *
 */

#include "m25p80.h"

#include <string.h> // memset
#include "spi.h"
#include "log.h"
#include "cmsis_os.h"

#define SPI_TIMEOUT  50u

#define OPCODE_WREN      0x06u // Write enable
#define OPCODE_WRDI      0x04u // Write disable
#define OPCODE_RDID      0x9Fu // Read identification
#define OPCODE_RDSR      0x05u // Read status register
#define OPCODE_WRSR      0x01u // Write status register
#define OPCODE_READ      0x03u // Read data bytes
#define OPCODE_FAST_READ 0x0Bu // Read data bytes at higher speed
#define OPCODE_PP        0x02u // Page program
#define OPCODE_SE        0xD8u // Sector erase
#define OPCODE_BE        0xC7u // Bulk erase
#define OPCODE_DP        0xB9u // Deep power-down
#define OPCODE_RES       0xABu // Release from deep power-down

#define M25P80_SIGNATURE  0x13u
#define GET_BYTE_N(w, n) ((uint8_t)(w >> (8 * n)))

#define RD_LEN       0x04u
#define WR_LEN       0x05u

#define INPUT_BUFFER_SIZE   (256 + 4 + 4 + 1)
#define OUTPUT_BUFFER_SIZE  (256 + 4 + 4 + 1)
uint8_t in  [INPUT_BUFFER_SIZE] = {0};
uint8_t out[OUTPUT_BUFFER_SIZE] = {0};

static bool flash_wip(void);
static bool flash_write_enable(void);
static flash_status_t flash_get_status(void);

static flash_status_t byte_to_status(uint8_t byte);

static void delay_ms(uint32_t ms);
static bool spi_transmit(uint8_t *out, size_t out_len);
static void spi_transceive(uint8_t *out, uint8_t *in, size_t out_len, size_t in_len);

void flash_init() {
    FLASH_SET_CS;
}

void flash_wakeup() {
    uint8_t data[1];
    data[0] = OPCODE_RES;
    spi_transmit(data, 1);
}

void flash_power_down() {
    uint8_t data[1];
    data[0] = OPCODE_DP;
    spi_transmit(data, 1);
}

bool flash_verify_signature() {
    uint8_t data[4];
    uint8_t res[1];
    data[0] = OPCODE_RES;
    data[1] = 0x00;
    data[2] = 0x00;
    data[3] = 0x00;
    spi_transceive(data, res, 4, 1);
    return res[0] == M25P80_SIGNATURE;
}

bool flash_is_write_busy() {
    uint8_t data[1];
    uint8_t res[1];
    data[0] = OPCODE_RDSR;
    spi_transceive(data, res, 1, 1);
    return (res[0] & 0x01);
}

static flash_status_t flash_get_status() {
    flash_status_t status;
    memset(out, 0, OUTPUT_BUFFER_SIZE * sizeof(out[0]));
    memset(in,  0, INPUT_BUFFER_SIZE  * sizeof(in[0]));
    out[0] = OPCODE_RDSR;
    size_t out_len = 2;
    spi_transceive(out, in, out_len, INPUT_BUFFER_SIZE);
    status = byte_to_status(in[1]);
    return status;
}

static bool flash_write_enable() {
    flash_status_t status;
    bool res;
    delay_ms(10);
    memset(out, 0, OUTPUT_BUFFER_SIZE * sizeof(out[0]));
    out[0] = OPCODE_WREN;
    size_t out_len = 1;
    spi_transmit(out, out_len);
    delay_ms(10);
    status = flash_get_status();
    res = status.wel ? true : false;
    delay_ms(10);
    return res;
}

bool flash_erase_bulk() {

    while (!flash_wip());
    while (!flash_write_enable());

    uint8_t data[1];
    data[0] = OPCODE_BE;

    spi_transmit(data, 1);

    while (!flash_wip());
    while(flash_is_write_busy()) {
        //HAL_IWDG_Refresh(&hiwdg);
    }
    
    return true;
}

void flash_write(uint8_t *buffer, size_t buffer_len, uint32_t pageAddress) {

    while (!flash_wip());
    while (!flash_write_enable());

    memset(out, 0, OUTPUT_BUFFER_SIZE * sizeof(out[0]));
    out[0] = OPCODE_PP;
    out[1] = GET_BYTE_N(pageAddress, 2);
    out[2] = GET_BYTE_N(pageAddress, 1);
    out[3] = GET_BYTE_N(pageAddress, 0);

    size_t out_len = 4 + (buffer_len / sizeof(out[0]));

    memcpy(out + 4, buffer, buffer_len);

    spi_transceive(out, in, out_len, 0);

    while(flash_is_write_busy()) {
        //HAL_IWDG_Refresh(&hiwdg);
    }
}

void flash_read(uint8_t *buffer, size_t buffer_len, uint32_t pageAddress) {
    memset(out, 0, OUTPUT_BUFFER_SIZE * sizeof(out[0]));
    memset(in,  0, INPUT_BUFFER_SIZE * sizeof(in[0]));
    out[0] = OPCODE_READ;
    out[1] = GET_BYTE_N(pageAddress, 2);
    out[2] = GET_BYTE_N(pageAddress, 1);
    out[3] = GET_BYTE_N(pageAddress, 0);
    size_t in_len = RD_LEN + buffer_len;
    spi_transceive(out, in, RD_LEN, in_len);
    memcpy(buffer, in, buffer_len * sizeof(in[0]));
}

static bool flash_wip() {
    flash_status_t status;
    bool res;
    delay_ms(10);
    status = flash_get_status();
    res = status.wel ? false : true;
    delay_ms(10);
    //HAL_IWDG_Refresh(&hiwdg);
    return res;
}

static flash_status_t byte_to_status(uint8_t byte) {
    flash_status_t status;
    status.srwd    = byte >> 7;
    status.unused1 = byte >> 6;
    status.unused2 = byte >> 5;
    status.bp2     = byte >> 4;
    status.bp1     = byte >> 3;
    status.bp0     = byte >> 2;
    status.wel     = byte >> 1;
    status.wip     = byte >> 0;
    return status;
}

/*
    Wrapper for System-specific implementation.
*/
static void delay_ms(uint32_t ms) {
    osDelay(ms);
}

/*
    Wrapper for System-specific implementation.

    Expecting to send `len` bytes out of the `out`
    buffer simultaneously receiving `len` bytes
    in the `in` buffer.
*/
static void spi_transceive(uint8_t *out, uint8_t *in, size_t out_len, size_t in_len) {
    FLASH_RESET_CS;
    if (HAL_SPI_Transmit(&hspi3, out, out_len, SPI_TIMEOUT) != HAL_OK) {
        FLASH_SET_CS;
        return;
    }

    if (HAL_SPI_Receive(&hspi3, in, in_len, SPI_TIMEOUT) != HAL_OK) {
        FLASH_SET_CS;
        return;
    }
    FLASH_SET_CS;
}

static bool spi_transmit(uint8_t *out, size_t out_len) {
    FLASH_RESET_CS;
    if (HAL_SPI_Transmit(&hspi3, out, out_len, SPI_TIMEOUT) != HAL_OK) {
        FLASH_SET_CS;
        return false;
    }
    FLASH_SET_CS;
    return true;
}