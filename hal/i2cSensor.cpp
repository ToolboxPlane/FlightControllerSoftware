#include "i2cSensor.hpp"

I2cSensor::I2cSensor(I2C &_i2c, uint8_t addr) : i2c(_i2c){
    this->addr = addr;
}

uint16_t I2cSensor::getWord(uint8_t addr, ByteOrder order){
    char buf[] = {addr, 0};
    i2c.write(addr, buf, 1);
    i2c.read(addr, buf, 2);

    if(order == BIG_ENDIAN){
        return ((uint16_t)buf[0]) << 8 | buf[1];
    } else {
        return ((uint16_t)buf[1]) << 8 | buf[0];
    }
}

uint8_t I2cSensor::getByte(uint8_t addr){
    char buf[] = {addr};
    i2c.write(addr, buf, 1);
    i2c.read(addr, buf, 1);

    return buf[0];
}
