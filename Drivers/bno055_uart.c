/**
 * @file bno_055_uart.c
 * @author paul
 * @date 17.04.19
 * @brief bno_055_uart @TODO
 */

#include "bno055_uart.h"
#include <stdint.h>
#include <stdbool.h>

#define BUF_SIZE 32

typedef enum {
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
    receive_char_timeout = 0x0A
} bno055_response_t;

bool transaction_finished = false;
uint8_t *receive_buf;
bno055_response_t response;

void uart_callback(uint8_t data) {
    static uint8_t byte_in_message = 0;
    static bool acknowledge_or_failure = false;
    static uint8_t still_to_read = 0;

    switch (byte_in_message) {
        case 0: // Header
            if (data == 0xEE) {
                acknowledge_or_failure = true;
                byte_in_message = 1;
            } else if (data == 0xBB) {
                acknowledge_or_failure = false;
                byte_in_message = 1;
            } else {
                byte_in_message = 0;
            }
            break;
        case 1:
            if (acknowledge_or_failure) {
                response = data;
                byte_in_message = 0;
            } else {
                response = read_success;
                still_to_read = data;
                byte_in_message = 2;
            }
            break;
        default:
            receive_buf[byte_in_message - 2] = data;
            --still_to_read;
            ++byte_in_message;
            if (still_to_read == 0) {
                byte_in_message = 0;
                transaction_finished = true;
            }
            break;

    }
}

bool bno055_write_register(uint8_t reg, const uint8_t *data, uint8_t len) {
    uint8_t buf[len + 4];
    buf[0] = 0xAA;
    buf[1] = 0x00;
    buf[2] = reg;
    buf[3] = len;
    for (uint8_t c = 0; c<len; c++) {
        buf[c + 4] = data[c];
    }
    //uart2_send_buf(buf, len+4);
    while (!transaction_finished);
    return response == write_success;
}

bool bno055_read_register(uint8_t reg, uint8_t *data, uint8_t len) {
    receive_buf = data;
    uint8_t buf[4] = {0xAA, 0x01, reg, len};
    //uart2_send_buf(buf, 4);
    while (!transaction_finished);
    return response == read_success;
}

void bno055_init(void) {
    //uart2_init(115200, &uart_callback);
}
