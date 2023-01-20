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
 *  * Start the timer 2 with a period of 4.096ms
 *  * Set the watchdog to 30ms
 *
 * This function will start both timers, inside the timer callbacks the following tasks are performed:
 *  * For the low priority timer:
 *      * Enable nested interrupts by setting the I bit in the SREG register
 *      * Call the low-priority callback
 *      * Check the runtime, if it exceeds 12ms call
 *        error_handler_handle_warning(ERROR_HANDLER_GROUP_SYSTEM, SYSTEM_ERROR_RUNTIME_LOW_PRIO)
 *  * For the low priority timer:
 *      * Call the high-priority callback
 *      * Check the runtime, if it exceeds 1ms call
 *        error_handler_handle_warning(ERROR_HANDLER_GROUP_SYSTEM, SYSTEM_ERROR_RUNTIME_HIGH_PRIO)
 */
void system_post_init(void);

/**
 * @brief Reset the watchdog timer.
 *
 * This function calls wdt_reset.
 */
void system_reset_watchdog(void);

#endif // FLIGHTCONTROLLER_SYSTEM_H
