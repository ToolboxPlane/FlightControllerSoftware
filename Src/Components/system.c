/**
 * @file system.c
 * @author Paul Nykiel
 * @date 01.12.22
 * @brief Implementation of the system interface component.
 * @ingroup Components
 */
#include "system.h"

#include <Application/error_handler.h>
#include <HAL/timer8bit.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/wdt.h>

enum { MAX_TIME_SLOT_USAGE = (uint8_t) (3.5 / 4.096 * 255) };

static system_timer_4_096ms_callback_t callback;

static void internal_timer_callback(void) {
    callback();
    uint8_t time = timer_8bit_get_count();
    if (time > MAX_TIME_SLOT_USAGE) {
        error_handler_handle_warning(ERROR_HANDLER_GROUP_SYSTEM, SYSTEM_ERROR_TIMER_RUNTIME);
    }
}

void system_pre_init(system_timer_4_096ms_callback_t timer_callback) {
    cli();

    callback = timer_callback;
    if (MCUSR & (1U << WDRF)) {
        error_handler_handle_warning(ERROR_HANDLER_GROUP_SYSTEM, SYSTEM_ERROR_WATCHDOG);
    } else if (MCUSR & (1U << BORF)) {
        error_handler_handle_warning(ERROR_HANDLER_GROUP_SYSTEM, SYSTEM_ERROR_BROWNOUT);
    }
    MCUSR = 0;

    wdt_enable(WDTO_250MS);
    sei();
}

void system_post_init(void) {
    //  Runs at 16.384ms interval, the BNO055 provides data at 100Hz, the output can be updated at 50Hz
    timer_8bit_init(prescaler_256, &internal_timer_callback);

    wdt_enable(WDTO_30MS);
}

void system_reset_watchdog(void) {
    wdt_reset();
}
