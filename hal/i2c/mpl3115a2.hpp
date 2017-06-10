#ifndef _MPL3115A2_H
#define _MPL3115A2_H

#include "mbed.h"

class Mpl3115a2{
    public:
        Mpl3115a2(I2C &_i2c, uint8_t addr=0xC0);
        uint8_t isReady();
        float getAltitude();
        uint8_t isAvailable();
    private:
        I2C i2c;
        uint8_t addr;
};

#endif
