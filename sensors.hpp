#include "mbed.h"

enum HeightSource{
    US, BARO
};

namespace sensors{
    Ticker ticker;
    I2C i2c(I2C_SDA, I2C_SCL);

    Bno055 imu(i2c);
    Mpl3115a2 baro(i2c);
    Srf02 us(i2c);

    uint16_t initialHeight;
    int16_t height;
    HeightSource heightSource;

    uint8_t init(){
        if(!imu.isAvailable() || !us.isAvailable() || !baro.isAvailable())
            return false;

        us.startMeasurement();
        while(!baro.isReady());
        initialHeight = baro.getAltitude();

        ticker.attach(&tick, 1.0/ISR_FREQ);
        return true;
    }



    void tick(void){
        static uint16_t usDist = 1000;
        static uint16_t baroHeight = 0;
        static uint8_t validUsDistance = false;

        // Height calculations
        if(us.isReady()){
            usDist = us.readDistance();
            us.startMeasurement();
        }
        if(baro.isReady()){
            baroHeight = baro.getAltitude();
        }

        if(usDist < US_MAX_RANGE - US_PRECISION ||
            usDist > US_MAX_RANGE + US_PRECISION && validUsDistance){
            validUsDistance = true;

            heightSource = US;
            height = usDist;
        } else{
            height = baroHeight - initialHeight;

            heightSource = BARO;
            validUsDistance = false;
        }
    }
}
