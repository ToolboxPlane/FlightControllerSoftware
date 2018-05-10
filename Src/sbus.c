//
// Created by paul on 14.04.18.
//

#include <sbus.h>
#include "sbus.h"

#define SBUS_START_BYTE 0x0F
#define SBUS_END_BYTE 0x00

static sbus_data_t new_data;
sbus_data_t sbus_latest_data = {{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, true, true};

bool sbus_parse(const uint8_t *data, uint8_t len) {
    static uint8_t byteCount = 0;
    bool ret = false;

    for(uint8_t c=0; c<len; c++) {
        switch (byteCount) {
            case 0: // Startbyte
                if (data[c] == SBUS_START_BYTE) {
                    byteCount = 1;
                }
                break;
            case 24: // Endbyte
                if (data[c] == SBUS_END_BYTE) {
                    for(uint8_t c=0; c<16; c++) {
                        sbus_latest_data.channel[c] = new_data.channel[c];
                        new_data.channel[c] = 0;
                    }
                    sbus_latest_data.frame_lost = new_data.frame_lost;
                    sbus_latest_data.failsave =  new_data.failsave;

                    ret = true;
                }
                byteCount = 0;
                break;
            case 23: // Flags
                new_data.failsave = (data[c] >> 4) & 1;
                new_data.frame_lost = (data[c] >> 5) & 1;
                byteCount =  24;
                break;
            default: // Data
            {
                uint16_t startBitNum = (byteCount - 1) * 8;

                for(uint8_t b=0; b<8; b++) {
                    uint8_t channelNumber = (startBitNum + b) / 11;
                    uint8_t bitInChannel = ((startBitNum + b) % 11);
                    uint8_t bit = (data[c] >> b) & 1;
                    new_data.channel[channelNumber] |= bit << bitInChannel;
                }
                byteCount++;
            }
            break;
        }
    }
    return ret;
}
