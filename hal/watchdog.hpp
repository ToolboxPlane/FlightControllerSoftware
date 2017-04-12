#ifndef _WATCHDOG_H
#define _WATCHDOG_H

#include "mbed.h"

class Watchdog{
    public:
        Watchdog(uint16_t ms);
        void reset();
    private:
        Timeout timeout;
        uint16_t ms;
        void overflow();
};

#endif
