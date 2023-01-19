/**
 * @file system.h
 * @author Paul Nykiel
 * @date 01.12.22
 * @brief Declaration of the system interface component.
 * @ingroup Components
 */
#ifndef FLIGHTCONTROLLER_SYSTEM_H
#define FLIGHTCONTROLLER_SYSTEM_H

/**
 * Errors reported by the system component to the error-handler.
 */
typedef enum {
    SYSTEM_ERROR_WATCHDOG = 1,
    SYSTEM_ERROR_BROWNOUT = 2,
    SYSTEM_ERROR_RUNTIME_LOW_PRIO = 3,
    SYSTEM_ERROR_RUNTIME_HIGH_PRIO = 4,
} system_error_t;

/**
 * The type of function to be called by the system component every 16.384ms with low priority.
 */
typedef void (*system_timer_16_384ms_callback)(void);

/**
 * The type of function to be called by the system component every 4.096ms with high priority.
 */
typedef void (*system_timer_4_096ms_callback)(void);

/**
 * @brief Run initial system setup.
 *
 * The pre-initialization consists of the following tasks:
 *  * Disable all interrupts
 *  * Check the source of the last reset and trigger a warning if it was a watchdog or brownout
 *  * Set the watchdog to 250ms for component initialization
 *  * Re-enable the interrupts
 *
 * @param low_prio_callback low priority function to be called every 16.384ms
 * @param high_prio_callback high priority function to be called every 4.096ms
 */
void system_pre_init(system_timer_16_384ms_callback low_prio_callback,
                     system_timer_4_096ms_callback high_prio_callback);

/**
 * @brief Run system setup after component initialization.
 *
 * The post-initialization consists of the following task:
 *  * Start the timer 0 with a period of 16.384ms
 *  * Start the timer 2 with a period of 16.384ms
 *  * Set the watchdog to 30ms
 */
void system_post_init(void);

/**
 * @brief Reset the watchdog timer.
 *
 * This function calls wdt_reset.
 */
void system_reset_watchdog(void);

#endif // FLIGHTCONTROLLER_SYSTEM_H
