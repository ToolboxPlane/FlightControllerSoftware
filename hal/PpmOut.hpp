#ifndef _PPM_OUT_H
#define _PPM_OUT_H

#include "mbed.h"

class PpmOut{
    public:
        PpmOut(PinName pin);
        void setValue(uint16_t val);
    private:
        PwmOut out;
};
#endif