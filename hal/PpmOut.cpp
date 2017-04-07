#include "PpmOut.hpp"
#include "mbed.h"

PpmOut::PpmOut(PinName pin) : out(pin){
    out.period_ms(20);
    out.pulsewidth_us(0);
}

void PpmOut::setValue(uint16_t val){
    out.pulsewidth_us(1000+val);   
}