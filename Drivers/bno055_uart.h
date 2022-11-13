/**
 * @file bno_055_uart.h
 * @author paul
 * @date 17.04.19
 * @brief bno_055_uart @TODO
 */

#ifndef FLIGHTCONTROLLER_BNO055_UART_H
#define FLIGHTCONTROLLER_BNO055_UART_H

#include <stdint.h>

#define BNO_UART_ID 1

typedef enum {
    // From BNO
    read_success = 0x00,
    write_success = 0x01,
    read_fail = 0x02,
    write_fail = 0x03,
    regmap_invalid_address = 0x04,
    regmap_write_disabled = 0x05,
    wrong_start_byte = 0x06,
    bus_over_run_error = 0x07,
    max_length_error = 0x08,
    min_length_error = 0x09,
    receive_char_timeout = 0x0A,

    // From Library
    callback_buffer_invalid,
    callback_length_invalid
} bno055_response_t;

typedef void (*bno_callback_t)(bno055_response_t);

void bno055_uart_init(void);

void bno055_uart_write_register(uint8_t reg, const uint8_t *data, uint8_t len, bno_callback_t callback, void *result, uint8_t div);

void bno055_uart_read_register(uint8_t reg, uint8_t len, bno_callback_t callback, void *result, uint8_t div);


#endif // FLIGHTCONTROLLER_BNO055_UART_H
