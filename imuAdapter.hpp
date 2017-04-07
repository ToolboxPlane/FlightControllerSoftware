#ifndef _IMU_ADAPTER_H
#define _IMU_ADAPTER_H

#include "mbed.h"

enum Axis{
    AX_X = 0,
    AX_Y = 1,
    AX_Z = 2
};

class ImuAdapter{
    public:
        virtual float getAngle(Axis a) = 0; 
};

#endif