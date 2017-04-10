#ifndef _I2C_SENSOR_H
#define _I2C_SENSOR_H

#include "mbed.h"

enum ByteOrder{
    BIG_ENDIAN,
    LITTLE_ENDIAN
};

class I2cSensor{
    public:
        I2cSensor(I2C &_i2, uint8_t addr);
    protected:
        I2C i2c;
        uint8_t addr;

        uint16_t getWord(uint8_t addr, ByteOrder order);
        uint8_t getByte(uint8_t addr);
};

#endif
