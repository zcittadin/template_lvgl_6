/*
 * Copyright (c) 2019 Coopermaq
 * All rights reserved
 */

#ifndef UTIL_H
#define UTIL_H

uint8_t crc8(uint8_t *addr, uint8_t len);
uint16_t crc16(const uint8_t *data, uint16_t len);
uint16_t humidity_rh_to_bh(uint16_t temperature, uint16_t rh_humidity);
uint16_t byte_to_word(uint8_t byte_h, uint8_t byte_l);
void word_to_byte(uint16_t word, uint8_t *arr_byte);
long byte_to_long(uint8_t *arr_byte);
void long_to_byte(long long_num, uint8_t *arr_byte);
void long_to_word(long value, uint16_t *arr_word);
char* get_value_at(char* data, int pos, char* separator);
void trim_digit(char *digit);

uint16_t set_bit(uint16_t word, uint8_t position);
uint16_t clear_bit(uint16_t word, uint8_t position);
uint16_t toggle_bit(uint16_t word, uint8_t position);
uint8_t test_bit(uint16_t word, uint8_t position);

#endif /* UTIL_H */
