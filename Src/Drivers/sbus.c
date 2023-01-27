/**
 * @file sbus.c
 * @author Paul Nykiel
 * @date 14.04.18
 * @brief Implementation of the library functions for sbus reception and decoding.
 * @ingroup Drivers
 */

#include "sbus.h"

#include <HAL/uart.h>

#include "ring_buffer.h"

enum { UART_ID = 2 };
enum { UART_BAUD = 100000 };

enum { SBUS_START_BYTE = 0x0F };
enum { SBUS_END_BYTE = 0x00 };
enum { SBUS_BITS_PER_CHANNEL = 11U };

static ring_buffer_data_t ring_buffer_data;
static sbus_data_t latest_data;
static sbus_data_t curr_decode_data;
static uint8_t decode_byte_count;

static void uart_callback(uint8_t data) {
    if (ring_buffer_put(&ring_buffer_data, data)) {
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
    decode_byte_count = 0;
    latest_data.failsafe = true;
    latest_data.frame_lost = true;
    ring_buffer_data = ring_buffer_init();
    uart_init(UART_ID, UART_BAUD, EVEN, 2, uart_callback);
}

bool sbus_data_available(void) {
    bool res = false;
    uint8_t data = 0;
    while (ring_buffer_get(&ring_buffer_data, &data)) {
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
