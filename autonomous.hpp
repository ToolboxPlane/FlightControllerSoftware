#include "mbed.h"
#include "flightControl.hpp"

void brick(){
    flightControl::roll(0);
    flightControl::pitch(0);
    flightControl::power(0);
}

void levelFlight(int16_t roll, int16_t pitch, int16_t power){
    int16_t deltaRoll = sensors::roll - roll;
    int16_t deltaPitch = sensors::pitch - pitch;


    flightControl::roll(deltaRoll * 15);
    flightControl::pitch(deltaPitch * 15);
    flightControl::power(power);
}
