/**
 * @file system.h
 * @author paul
 * @date 01.12.22
 * Description here TODO
 */
#ifndef FLIGHTCONTROLLER_SYSTEM_H
#define FLIGHTCONTROLLER_SYSTEM_H

typedef void (*timer_callback_t)(void);

void system_init(timer_callback_t timer_callback);

void system_reset_watchdog(void);

#endif // FLIGHTCONTROLLER_SYSTEM_H
