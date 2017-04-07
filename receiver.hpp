/**
* @brief A Adapter which converts SBUS Signals to PPM Signals with Automatic Calibration
* @author Paul Nykiel
*/

#ifndef _RECEIVER_H
#define _RECEIVER_H

#include "mbed.h"
#include "hal/SBus.hpp"

namespace receiver{
    struct MinMax{
        uint16_t min;
        uint16_t max;
    };

    SBus sbus(SBUS_RX, SBUS_TX);
    MinMax mm[5];

    void init(){
        for(int c=0; c<3; c++){
            mm[c].min = 1024;
            mm[c].max = 1024;
        }
    }

    uint16_t normalizePpm(uint16_t val, uint16_t min, uint16_t max){
        return (float)(val-min)/(max-min) * 1000;
    }

    uint16_t get(uint8_t c){
        if(c < 4){
            uint16_t val = sbus.getChannel(c);

            if(val < mm[c].min)
                mm[c].min = val;
            else if(val > mm[c].max)
                mm[c].max = val;

            return normalizePpm(val, mm[c].min, mm[c].max);
        }else if(c<16){
            return sbus.getChannel(c) > 1024;
        }else{
            return 0;
        }
    }
}



#endif
