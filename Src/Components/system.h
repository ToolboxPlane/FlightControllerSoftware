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
typedef enum { SYSTEM_ERROR_TIMER_RUNTIME = 1, SYSTEM_ERROR_WATCHDOG = 2, SYSTEM_ERROR_BROWNOUT = 3 } system_error_t;

/**
 * The type of function to be called by the system component.
 */
typedef void (*system_timer_16_384ms_callback)(void);

/**
 * @brief Run initial system setup.
 *
 * The pre-initialization consists of the following tasks:
 *  * Disable all interrupts
 *  * Check the source of the last reset and trigger a warning if it was a watchdog or brownout
 *  * Set the watchdog to 250ms for component initialization
 *  * Re-enable the interrupts
 *
 * @param timer_callback the function to be called by the timer.
 */
void system_pre_init(system_timer_16_384ms_callback timer_callback);

/**
 * @brief Run system setup after component initialization.
 *
 * The post-initialization consists of the following task:
 *  * Start the timer with a period of 16.384ms
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
