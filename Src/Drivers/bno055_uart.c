/**
 * @file bno055_uart.c
 * @author Paul Nykiel
 * @date 17.04.19
 * @brief Implementation of the library functions for the UART communication with the Bosch Sensortec BNO-055.
 * @ingroup Drivers
 */

#include "bno055_uart.h"

#include <HAL/uart.h>
#include <stdbool.h>

enum { BNO_UART_ID = 1 };
enum { BNO_BAUD = 115200U };
enum { BNO_SEND_START = 0xAA };
enum { BNO_RECEIVE_START = 0xBB };
enum { BNO_RECEIVE_ERROR = 0xEE };

static volatile bno055_callback_t bno_callback;
static volatile void *bno_result_data;

static void bno_uart_callback(uint8_t data) {
    static uint8_t byte_in_message = 0;
    static uint8_t payload_len = 0;
    static bool acknowledge_or_failure = false;

    switch (byte_in_message) {
        case 0: // Header
            if (data == BNO_RECEIVE_ERROR) {
                acknowledge_or_failure = true;
                byte_in_message = 1;
            } else if (data == BNO_RECEIVE_START) {
                acknowledge_or_failure = false;
                byte_in_message = 1;
            } else {
                byte_in_message = 0;
                bno_callback(invalid_sync);
            }
            break;
        case 1:
            if (acknowledge_or_failure) {
                byte_in_message = 0;
                bno_callback(data);
            } else {
                payload_len = data;
                byte_in_message = 2;
            }
            break;
        default:
            *((uint8_t *) bno_result_data) = data;
            bno_result_data += 1;
            payload_len -= 1;

            if (payload_len == 0) {
                byte_in_message = 0;
                bno_callback(read_success);
            } else {
                byte_in_message += 1;
            }
            break;
    }
}

void bno055_uart_init(void) {
    uart_init(BNO_UART_ID, BNO_BAUD, NONE, 1, &bno_uart_callback);
}

void bno055_uart_write_register(uint8_t reg, const uint8_t *data, uint8_t len, bno055_callback_t callback) {
    bno_callback = callback;

    uint8_t buf[len + 4];
    buf[0] = BNO_SEND_START;
    buf[1] = 0x00;
    buf[2] = reg;
    buf[3] = len;
    for (uint8_t index = 0; index < len; index++) {
        buf[index + 4] = data[index];
    }
    uart_send_buf(BNO_UART_ID, buf, len + 4);
}

void bno055_uart_read_register(uint8_t reg, uint8_t len, bno055_callback_t callback, void *result) {
    bno_callback = callback;
    bno_result_data = result;

    uint8_t buf[4] = {BNO_SEND_START, 0x01, reg, len};
    uart_send_buf(BNO_UART_ID, buf, 4);
}
