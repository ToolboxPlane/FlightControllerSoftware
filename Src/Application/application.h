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
 *  * System Pre-Initialization (::system_pre_init)
 *  * Error Handler initialization (::error_handler_init)
 *  * IMU initialization (::imu_init)
 *  * Remote initialization (::remote_init)
 *  * Flight-computer initialization (::flight_computer_init)
 *  * Actuator initialization (::actuators_init)
 *  * Mode handler initialization (::mode_handler_init)
 *  * System Post-Initialization (::system_post_init)
 *
 * The system post initialization will start the timer, which is also implemented as part of the application logic.
 * This function never returns but calls wdt_reset in an infinite loop, if this idle does not run for 30ms
 * (i.e. the system is continuously blocked by interrupts) a reset is performed.
 *
 * The timer performs the following tasks:
 *  * Call the mode handler (::mode_handler_handle) to determine the current mode
 *  * Calculate the actuator commands depending on the mode:
 *      * In flight-computer mode: call the controller (::controller_update) with the flight-computer set point and the
 *          imu data and use the result as elevon commands, the motor command is taken directly from the flight-computer
 *          set point
 *      * In remote mode: take the remote message as actuator command, remapping the elevons from
 *          [0, 1000] to [-500,500]
 *      * In stabilised failsafe mode: call the controller with roll: 0, pitch: 0 as set point and the imu data
 *           and use the result as elevon commands, the motor command is always 0
 *      * In failsafe mode: set all three commands to 0
 *  * Pass the actuator commands to the actuators (::actuators_set)
 *  * Every FLIGHT_COMPUTER_SEND_PERIOD frame: pass the current data to ::flight-computer_send
 *  * Call ::imu_start_sampling
 */
void application_init(void);

#endif // FLIGHTCONTROLLER_APPLICATION_H
