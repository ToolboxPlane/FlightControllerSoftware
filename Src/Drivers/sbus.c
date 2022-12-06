/**
 * @file sbus.c
 * @author Paul Nykiel
 * @date 14.04.18
 * @brief Implementation of the library functions for sbus reception and decoding.
 */

#include "sbus.h"

#include <HAL/uart.h>

enum { SBUS_UART = 2 };
enum { SBUS_START_BYTE = 0x0F };
enum { SBUS_END_BYTE = 0x00 };

static volatile sbus_data_t sbus_datas[2];
static volatile uint8_t curr_sampling_data = 0;
static volatile bool sbus_sampling_complete;

static void sbus_uart_callback(uint8_t data) {
    static uint8_t byte_count = 0;

    sbus_data_t *sbus_data = (sbus_data_t *) (&sbus_datas[curr_sampling_data]);

    switch (byte_count) {
        case 0: // Startbyte
            if (data == SBUS_START_BYTE) {
                byte_count = 1;
            }
            for (uint8_t index = 0; index < 16; ++index) {
                sbus_data->channel[index] = 0;
            }
            break;
        case 24: // Endbyte
            if (data == SBUS_END_BYTE) {
                curr_sampling_data = 1 - curr_sampling_data;
                sbus_sampling_complete = true;
            } else {
                // TODO warning
            }
            byte_count = 0;
            break;
        case 23: // Flags
            sbus_data->failsave = (data >> 3u) & 1u;
            sbus_data->frame_lost = (data >> 2u) & 1u;
            byte_count = 24;
            break;
        default: // Data
        {
            uint16_t startBitNum = (byte_count - 1) * 8;

            for (uint8_t bit_index = 0; bit_index < 8; bit_index++) {
                uint8_t channelNumber = (startBitNum + bit_index) / 11;
                uint8_t bitInChannel = ((startBitNum + bit_index) % 11);
                uint8_t bit = (data >> bit_index) & 1u;
                sbus_data->channel[channelNumber] |= bit << bitInChannel;
            }
            byte_count++;
        } break;
    }
}

void sbus_init(void) {
    uart_init(SBUS_UART, 100000, EVEN, 2, sbus_uart_callback);
    curr_sampling_data = 0;
    sbus_datas[1 - curr_sampling_data].frame_lost = true;
    sbus_datas[1 - curr_sampling_data].failsave = true;
    sbus_sampling_complete = false;
}

sbus_data_t sbus_get_latest_data(void) {
    sbus_sampling_complete = false;
    return sbus_datas[1 - curr_sampling_data];
}

bool sbus_data_available(void) {
    return sbus_sampling_complete;
}
