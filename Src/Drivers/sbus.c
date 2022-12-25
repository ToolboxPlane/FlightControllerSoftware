/**
 * @file sbus.c
 * @author Paul Nykiel
 * @date 14.04.18
 * @brief Implementation of the library functions for sbus reception and decoding.
 * @ingroup Drivers
 */

#include "sbus.h"

#include <HAL/uart.h>

enum { UART_ID = 2 };
enum { UART_BAUD = 100000 };

enum { RX_BUF_SIZE = 512 };

enum { SBUS_START_BYTE = 0x0F };
enum { SBUS_END_BYTE = 0x00 };
enum { SBUS_BITS_PER_CHANNEL = 11U };

static volatile uint8_t rx_buf[RX_BUF_SIZE];
static volatile uint8_t rx_head, rx_tail;
static volatile sbus_data_t latest_data;
static volatile sbus_data_t curr_decode_data;
static volatile uint8_t decode_byte_count;

static void uart_callback(uint8_t data) {
    rx_buf[rx_head] = data;
    rx_head = (rx_head + 1) % RX_BUF_SIZE;
    if (rx_tail == rx_head) {
        // TODO error handling
    }
}

static bool sbus_decode(uint8_t data) {
    switch (decode_byte_count) {
        case 0: // Startbyte
            if (data == SBUS_START_BYTE) {
                decode_byte_count = 1;
            }
            for (uint8_t index = 0U; index < SBUS_NUM_CHANNELS; ++index) {
                curr_decode_data.channel[index] = 0;
            }
            break;
        case 24: // Endbyte
            decode_byte_count = 0;
            if (data == SBUS_END_BYTE) {
                return true;
            }
            // TODO warning
            break;
        case 23: // Flags
            curr_decode_data.failsafe = (data >> 3u) & 1u;
            curr_decode_data.frame_lost = (data >> 2u) & 1u;
            decode_byte_count = 24;
            break;
        default: // Data
        {
            uint16_t startBitNum = (decode_byte_count - 1) * 8;

            for (uint8_t bit_index = 0; bit_index < 8; bit_index++) {
                uint8_t channelNumber = (startBitNum + bit_index) / SBUS_BITS_PER_CHANNEL;
                uint8_t bitInChannel = ((startBitNum + bit_index) % SBUS_BITS_PER_CHANNEL);
                uint8_t bit = (data >> bit_index) & 1u;
                curr_decode_data.channel[channelNumber] |= bit << bitInChannel;
            }
            decode_byte_count++;
        } break;
    }
    return false;
}

void sbus_init(void) {
    uart_init(UART_ID, UART_BAUD, EVEN, 2, uart_callback);
    rx_head = 0;
    rx_tail = 0;
    decode_byte_count = 0;
    latest_data.failsafe = true;
    latest_data.frame_lost = true;
}

bool sbus_data_available(void) {
    bool res = false;
    while (rx_tail != rx_head) {
        uint8_t data = rx_buf[rx_tail];
        rx_tail = (rx_tail + 1) % RX_BUF_SIZE;

        if (sbus_decode(data)) {
            for (uint8_t index = 0U; index < SBUS_NUM_CHANNELS; ++index) {
                latest_data.channel[index] = curr_decode_data.channel[index];
            }
            latest_data.failsafe = curr_decode_data.failsafe;
            latest_data.frame_lost = curr_decode_data.frame_lost;
            res = true;
        }
    }
    return res;
}

sbus_data_t sbus_get_latest_data(void) {
    return latest_data;
}
