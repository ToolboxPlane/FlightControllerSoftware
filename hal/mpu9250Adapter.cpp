#include "mpu9250Adapter.hpp"

Mpu9250Adapter::Mpu9250Adapter(PinName mosi, PinName miso, PinName sck, PinName cs, uint16_t f)
    : spi(mosi, miso, sck), imu(spi, cs)
{
    ticker.attach(this, &Mpu9250Adapter::poll, 1.0/f);
    this->f = f;
    
    absValues[0] = absValues[1] = absValues[2] = 0;
}

void Mpu9250Adapter::poll(){
    imu.read_all();
        
    for(int c=0; c<3; c++){
        absValues[c] += imu.gyroscope_data[c]/f;
    }
}

float Mpu9250Adapter::getAngle(Axis a){
    return absValues[a];
}