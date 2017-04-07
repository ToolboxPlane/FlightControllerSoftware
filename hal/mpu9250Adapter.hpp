#ifndef _MPU_9250_ADAPTER_H
#define _MPU_9250_ADAPTER_H

#include "mbed.h"
#include "imuAdapter.hpp"
#include "MPU9250.h"


class Mpu9250Adapter: public ImuAdapter{
    public:
        Mpu9250Adapter(PinName mosi, PinName miso, PinName sck, PinName cs, uint16_t f = 50);
        float getAngle(Axis a);
    private:
        void poll();
        Ticker ticker;
        SPI spi;
        mpu9250_spi imu;
        uint16_t f;
        uint16_t absValues[3];
};

#endif