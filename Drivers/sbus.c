//
// Created by paul on 14.04.18.
//

#include "sbus.h"

#include <HAL/uart.h>

#define SBUS_UART 2

#define SBUS_START_BYTE 0x0F
#define SBUS_END_BYTE 0x00

static volatile sbus_data_t sbus_datas[2];
static uint8_t curr_sampling_data = 0;

static void sbus_uart_callback(uint8_t data) {
    static uint8_t byte_count = 0;

    sbus_data_t *sbus_data = (sbus_data_t *) (&sbus_datas[curr_sampling_data]);

    switch (byte_count) {
        case 0: // Startbyte
            if (data == SBUS_START_BYTE) {
                byte_count = 1;
            }
            for (uint8_t c = 0; c < 16; ++c) {
                sbus_data->channel[c] = 0;
            }
            break;
        case 24: // Endbyte
            if (data == SBUS_END_BYTE) {
                curr_sampling_data = 1 - curr_sampling_data;
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

            for (uint8_t b = 0; b < 8; b++) {
                uint8_t channelNumber = (startBitNum + b) / 11;
                uint8_t bitInChannel = ((startBitNum + b) % 11);
                uint8_t bit = (data >> b) & 1u;
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
}

sbus_data_t sbus_get_latest_data(void) {
    return sbus_datas[1 - curr_sampling_data];
}
