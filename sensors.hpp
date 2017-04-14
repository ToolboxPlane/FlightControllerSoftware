#include "mbed.h"

#include "hal/i2c/bno055.hpp"
#include "hal/i2c/srf02.hpp"
#include "hal/i2c/mpl3115a2.hpp"

#include "defines.hpp"

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
    int32_t height;
    HeightSource heightSource;
    int16_t roll, pitch, heading;
    uint8_t heightRequired = 0;
    uint8_t imuRequired = 0;

    void tick(void){
        static uint16_t usDist = 1000;
        static uint16_t baroHeight = 0;
        static uint8_t validUsDistance = false;
        static uint8_t imuSMS = 0;
        static uint8_t usTimer = 0;
        static uint8_t baroTimer = 0;

        // Height calculations
        if(heightRequired && usTimer++ == 10){
            usDist = us.readDistance();
        } else if(heightRequired && usTimer >= 11){
            us.startMeasurement();
            usTimer = 0;
        } else if(heightRequired && baroTimer++ >= 30){
            baroHeight = baro.getAltitude();
            baroTimer = 0;
        } else if(imuRequired) {
            // Orientation
            switch (imuSMS++) {
                case 0:
                    roll = imu.eulRoll();
                    break;
                case 1:
                    pitch = imu.eulPitch();
                    break;
                case 2:
                    heading = imu.eulHeading();
                    imuSMS = 0;
                    break;
                default:
                    imuSMS = 0;
                    break;
            }
        }
        if(heightRequired){
            if(usDist < US_MAX_RANGE - US_PRECISION ||
                (usDist > US_MAX_RANGE + US_PRECISION && validUsDistance)){
                validUsDistance = true;

                heightSource = US;
                height = usDist;
            } else{
                height = (baroHeight - initialHeight)*100;

                heightSource = BARO;
                validUsDistance = false;
            }
        }
    }

    uint8_t init(){
        us.startMeasurement();
        while(!baro.isReady());
        initialHeight = baro.getAltitude();

        ticker.attach(&tick, 1.0/ISR_FREQ);
        return true;
    }
}
