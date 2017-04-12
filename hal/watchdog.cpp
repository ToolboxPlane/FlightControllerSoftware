#include "watchdog.hpp"

extern "C" void mbed_reset();

Watchdog::(uint16_t ms){
    this->ms = ms;
    timeout.attach(this, &Watchdog::overflow, ms);
}

void Watchdog::reset(){
    timeout.attach(this, &Watchdog::overflow, ms);
}


// @TODO check
void Watchdog::overflow(){
    NVIC_SystemReset();
    mbed_reset();
}
