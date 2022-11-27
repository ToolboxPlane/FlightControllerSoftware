#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <stdbool.h>
#include <util/delay.h>

#include "Components/flightcomputer.h"
#include "Components/imu.h"
#include "Components/remote.h"
// TODO: Timer, LED, PPM, WDT, Error Handler

void timer_tick(void) {
    // Handle Ti
}

int main(void) {
    cli();
    /*output_init();
    output_led(0, on);
    output_led(1, MCUSR & (1u << WDRF) ? on : off); // Watchdog
    output_led(2, MCUSR & (1u << BORF) ? on : off); // Brownout
    MCUSR = 0;

    communication_init(&setpoint_update, &sbus_event);
    controller_init(16);
    //  Runs at 16.384ms interval, the BNO055 provides data at 100Hz, the output can be updated at 50Hz
    timer_8bit_init(prescaler_1024, &timer_tick);*/
    sei();

    wdt_enable(WDTO_30MS);

    if (!imu_init()) {
    }

    imu_start_sampling();

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
    while (true) {
        wdt_reset();
    }
#pragma clang diagnostic pop
}
