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
 * @brief Main application that performs initialization and starts the timer.
 *
 * The initialization consists of the following tasks:
 *  * System Pre-Initialization
 *  * Error Handler initialization
 *  * IMU initialization
 *  * Remote initialization
 *  * Flight-computer initialization
 *  * Actuator initialization
 *  * Mode handler initialization
 *  * System Post-Initialization
 *
 * The system post initialization will start the timer, which is also implemented as part of the application logic.
 * This function never returns but calls wdt_reset in an infinite loop, if this idle does not run for 30ms
 * (i.e. the system is continuously blocked by interrupts) a reset is performed.
 *
 * The timer performs the following tasks:
 *  * Call the mode handler to determine the current mode
 *  * Calculate the actuator commands depending on the mode:
 *      * In flight-computer mode: call the controller with the flight-computer setpoint and the imu data
 *          and use the result as elevon commands, the motor command is taken directly from the flight-computer setpoint
 *      * In remote mode: take the remote message as actuator command, remapping the elevons from
 *          [0, 1000] to [-500,500]
 *      * In stabilised failsafe mode: call the controller with roll: 0, pitch: 0 as setpoint and the imu data
 *           and use the result as elevon commands, the motor command is always 0
 *      * In failsafe mode: set all three commands to 0
 *  * Pass the actuator commands to the actuators
 *  * Every FLIGHTCOMPUTER_SEND_PERIOD frame: pass the current data to flight-computer_send
 *  * Call imu_start_sampling
 */
void application_init(void);

#endif // FLIGHTCONTROLLER_APPLICATION_H
