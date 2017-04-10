#ifndef _SRF_02_H
#define _SRF_02_H

#include "mbed.h"
#include "i2cSensor.hpp"

class Srf02 : public I2cSensor {
    public:
        Srf02(I2C &_i2c, uint8_t addr=0xE0);
        void startMeasurement();
        uint8_t isReady();
        uint16_t readDistance();
};

#endif
