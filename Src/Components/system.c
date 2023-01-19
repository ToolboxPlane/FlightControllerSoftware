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
#include <stdbool.h>

enum { MAX_TIME_SLOT_USAGE_LOW_PRIO = (uint8_t) (12 / 16.384 * 255) };
enum { MAX_TIME_SLOT_USAGE_HIGH_PRIO = (uint8_t) (1 / 4.096 * 255) };

static system_timer_16_384ms_callback low_prio_callback_;
static system_timer_4_096ms_callback high_prio_callback_;

static void internal_low_prio_callback(void) {
    SREG |= 1U << SREG_I; // Enable nested interrupts for this interrupt
    low_prio_callback_();

    uint8_t time = timer_8bit_get_count(TIMER_ID_0);
    if (time > MAX_TIME_SLOT_USAGE_LOW_PRIO) {
        error_handler_handle_warning(ERROR_HANDLER_GROUP_SYSTEM, SYSTEM_ERROR_RUNTIME_LOW_PRIO);
    }
}

static void internal_high_prio_callback(void) {
    high_prio_callback_();

    uint8_t time = timer_8bit_get_count(TIMER_ID_2);
    if (time > MAX_TIME_SLOT_USAGE_HIGH_PRIO) {
        error_handler_handle_warning(ERROR_HANDLER_GROUP_SYSTEM, SYSTEM_ERROR_RUNTIME_HIGH_PRIO);
    }
}

void system_pre_init(system_timer_16_384ms_callback low_prio_callback,
                     system_timer_4_096ms_callback high_prio_callback) {
    cli();

    low_prio_callback_ = low_prio_callback;
    high_prio_callback_ = high_prio_callback;

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
    timer_8bit_init(TIMER_ID_0, prescaler_1024, &internal_low_prio_callback);
    timer_8bit_init(TIMER_ID_2, prescaler_256, &internal_high_prio_callback);

    wdt_enable(WDTO_30MS);
}

void system_reset_watchdog(void) {
    wdt_reset();
}
