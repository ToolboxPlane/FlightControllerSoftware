#include "mpl3115a2.hpp"

Mpl3115a2::Mpl3115a2(I2C &_i2c, uint8_t addr) : I2cSensor(_i2c, addr){
    // Set to Altimeter and Oversampling
    char cmd[] = {0x26, 0b10111000}; //CTRL-Reg, Oversampling x128/Altimeter
    i2c.write(addr, cmd, 2);


    // Enable Data Ready Flags
    cmd[0] = 0x13;
    cmd[1] = 0b111;
    i2c.write(addr, cmd, 2);

    // Set active
    cmd[0] = 0x26;
    cmd[1] = 0b10111001;
    i2c.write(addr, cmd, 2);
}

uint8_t Mpl3115a2::isReady(){
    return getByte(0) & (0b1<<2); // Changed to <<2  because new Temperature Data is not interesting
}

uint16_t Mpl3115a2::getAltitude(){
    return getWord(0x01);
}
