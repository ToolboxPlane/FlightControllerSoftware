#ifndef _SBUS_H
#define _SBUS_H

#include "mbed.h"

#define SBUS_START_BYTE 0x0F
#define SBUS_END_BYTE 0x00

class SBus{
    public:
        SBus(PinName rx, PinName tx);
        uint16_t getChannel(uint8_t channel);
        uint8_t getStatusByte();
        uint8_t frameLost();
        uint8_t failSave();
        uint8_t getDigital(uint8_t channel);
    private:
        void rxCallback();
        Serial serial;
        uint16_t channels[16];
        uint8_t status;
        uint8_t rxData[23];
        uint8_t rxMux;
};

#endif