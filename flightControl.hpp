#include "mbed.h"

namespace flightControl{
    PpmOut motor(MAIN_MOTOR);
    PpmOut servoAileronRight(AILERON_RIGHT);
    PpmOut servoAileronLeft(AILERON_LEFT);
    PpmOut servoVTailRight(VTAIL_RIGHT);
    PpmOut servoVTailLeft(VTAIL_LEFT);

    void power(int16_t power){
        motor.setValue(power);
    }

    void roll(int16_t power){
        servoAileronLeft.setValue(500-power);
        servoAileronRight.setValue(500-power);
    }

    void pitch(int16_t power){
        servoVTailLeft.setValue(500-power);
        servoVTailRight.setValue(500+power);
    }

    void init(){
        servoAileronRight.setValue(500);
        servoAileronLeft.setValue(500);
        servoVTailRight.setValue(500);
        servoVTailLeft.setValue(500);

        motor.setValue(0);
        for(int c=0; c<60; c++){
            wait_ms(100);
        }
    }
}
