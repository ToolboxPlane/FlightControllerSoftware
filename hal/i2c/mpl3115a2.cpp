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

/**
 * Check if the pressure measurment is finished. This is espacially important
 * if Oversampling is set to a high value.
 * @return a non zero value if the reading is finished. Zero if the reading isn't finished
 */
uint8_t Mpl3115a2::isReady(){
    return getByte(0) & (0b1<<2); // CHANGED to <<2  because new Temperature Data is not interesting
}


/**
 * Read the altitude (in meters over sea-level) from the sensor.
 * @return the current altitude in meters over sea-level
 * @see isReady()
 */
uint16_t Mpl3115a2::getAltitude(){
    return getWord(0x01);
}


/**
 * Read the WHO_AM_I register of the I²C sensor
 * @return The values of the WHO_AM_I Register as a uint8_t
 */
uint8_t Mpl3115a2::getWhoAmI(){
    // CHANGED Check
    return getByte(0x0C);
}

/**
 * Check wheter the device is on the I²C Bus. If the device is available
 * the WHO_AM_I register should read as 0xC4
 * @return A non zero value if the device is available, zero if not.
 */
uint8_t Mpl3115a2::isAvailable(){
    return getWhoAmI() == 0xC4;
}
