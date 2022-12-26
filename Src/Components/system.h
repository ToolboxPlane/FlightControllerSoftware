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

typedef void (*system_timer_callback_t)(void);

void system_pre_init(system_timer_callback_t timer_callback);
void system_post_init(void);

void system_reset_watchdog(void);

#endif // FLIGHTCONTROLLER_SYSTEM_H
