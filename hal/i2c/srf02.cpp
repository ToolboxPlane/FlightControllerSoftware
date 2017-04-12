#include "srf02.hpp"

Srf02::Srf02(I2C &_i2c, uint8_t addr) : I2cSensor(_i2c, addr){}

void Srf02::startMeasurement(){
    char cmd[] = {0, 0x51}; // CMD Register, Range cm
    I2cSensor::i2c.write(addr, cmd, 2);
}

uint8_t Srf02::isReady(){
    /*
    * Read from Software Revision Register, is 255 while ranging
    */

    return getByte(0x00) != 255;
}

uint16_t Srf02::readDistance(){
    return getWord(0x02);
}

// CHANGED Check
uint8_t Srf02::isAvailable(){
    return getByte(0x01) == 0x80;
}
