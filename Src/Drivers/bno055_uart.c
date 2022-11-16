/**
 * @file bno_055_uart.c
 * @author paul
 * @date 17.04.19
 * @brief bno_055_uart @TODO
 */

#include "bno055_uart.h"

#include <HAL/uart.h>
#include <stdbool.h>

#define BNO_UART_ID 1


static volatile uint8_t receive_buf[512];
static volatile bno_callback_t bno_callback;
static volatile void *bno_result_data;
static volatile uint8_t bno_result_div;

static void bno_uart_handle_response(volatile const uint8_t *data, uint8_t len, bno055_response_t response) {
    if (response == read_success) {
        if (len == 0) {
            // No data read...
        } else {
            if (bno_result_data == 0) {
                bno_callback(callback_buffer_invalid);
                return;
            }

            if (len == 1) {
                volatile uint8_t *result8 = (volatile uint8_t *) bno_result_data;
                *result8 = (*data) / bno_result_div;
            } else if (len == 2) {
                volatile uint16_t *result16 = (volatile uint16_t *) bno_result_data;
                *result16 = (data[1] << 8u | data[0]) / bno_result_div;
            } else {
                bno_callback(callback_length_invalid);
                return;
            }
        }
        bno_callback(response);
    } else {
        bno_callback(response);
    }
}

static void bno_uart_callback(uint8_t data) {
    static uint8_t byte_in_message = 0;
    static uint8_t payload_len = 0;
    static bool acknowledge_or_failure = false;

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
                bno_uart_handle_response(0, 0, invalid_sync);
            }
            break;
        case 1:
            if (acknowledge_or_failure) {
                byte_in_message = 0;
                bno_uart_handle_response(0, 0, (bno055_response_t) data);
            } else {
                payload_len = data;
                byte_in_message = 2;
            }
            break;
        default:
            receive_buf[byte_in_message - 2] = data;
            ++byte_in_message;
            if (byte_in_message >= payload_len + 2) {
                byte_in_message = 0;
                bno_uart_handle_response(receive_buf, payload_len, read_success);
            }
            break;
    }
}

void bno055_uart_init(void) {
    uart_init(BNO_UART_ID, 115200, NONE, 1, &bno_uart_callback);
}

void bno055_uart_write_register(uint8_t reg, const uint8_t *data, uint8_t len, bno_callback_t callback) {
    bno_callback = callback;

    uint8_t buf[len + 4];
    buf[0] = 0xAA;
    buf[1] = 0x00;
    buf[2] = reg;
    buf[3] = len;
    for (uint8_t c = 0; c < len; c++) {
        buf[c + 4] = data[c];
    }
    uart_send_buf(BNO_UART_ID, buf, len + 4);
}

void bno055_uart_read_register(uint8_t reg, uint8_t len, bno_callback_t callback, void *result, uint8_t div) {
    bno_callback = callback;
    bno_result_data = result;
    bno_result_div = div;

    uint8_t buf[4] = {0xAA, 0x01, reg, len};
    uart_send_buf(BNO_UART_ID, buf, 4);
}
