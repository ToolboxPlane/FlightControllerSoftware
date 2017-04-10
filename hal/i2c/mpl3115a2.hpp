#ifndef _MPL3115A2_H
#define _MPL3115A2_H

#include "mbed.h"
#include "i2cSensor.hpp"

class Mpl3115a2 : public I2cSensor{
    public:
        Mpl3115a2(I2C &_i2c, uint8_t addr=0xC0);
        uint8_t isReady();
        uint16_t getAltitude();
};

#endif
