/**
 * @file application.h
 * @author Paul Nykiel
 * @date 31.12.22
 * @brief Declaration of the main application logic.
 * @ingroup Application
 */
#ifndef FLIGHTCONTROLLER_APPLICATION_H
#define FLIGHTCONTROLLER_APPLICATION_H

/**
 * Main application that performs the following task:
 *  * System Pre-Initialization
 *  * Error Handler initialization
 *  * IMU initialization
 *  * Remote initialization
 *  * Flightcomputer initialization
 *  * Actuator initialization
 *  * Mode handler initialization
 *  * System Post-Initialization
 * The system post initialization will start the timer, which is also implemented as part of the application logic.
 * This function never returns but calls wdt_reset in an infinite loop, if this idle does not run for 30ms
 * (i.e. the system is continuously blocked by interrupts a reset is performed).
 */
void application_init(void);

#endif // FLIGHTCONTROLLER_APPLICATION_H
