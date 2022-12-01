/**
 * @file system.c
 * @author paul
 * @date 01.12.22
 * Description here TODO
 */
#include "system.h"

#include <HAL/timer8bit.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>

static timer_callback_t callback;

static void internal_timer_callbacK(void) {
    callback();
    uint8_t time = timer_8bit_get_count();
    if (time > 200) {
        // TODO warn, find number
    }
}

void system_init(timer_callback_t timer_callback) {
    callback = timer_callback;

    cli();
    // Watchdog
    if (MCUSR & (1u << WDRF)) {
        //error_handler_handle_warning(SYSTEM, 1);
    } else if (MCUSR & (1u << BORF)) { // Brownout
        //error_handler_handle_warning(SYSTEM, 2);
    }
    MCUSR = 0;

    //  Runs at 16.384ms interval, the BNO055 provides data at 100Hz, the output can be updated at 50Hz
    timer_8bit_init(prescaler_1024, &internal_timer_callbacK);
    sei();

    wdt_enable(WDTO_30MS);
}

void system_reset_watchdog(void) {
    wdt_reset();
}
