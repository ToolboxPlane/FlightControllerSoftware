/**
 * @file main.c
 * @author Paul Nykiel
 * @date 12.04.19
 * @brief Main file of the Flightcontroller firmware.
 */

/**
 * @defgroup HAL Hardware-Abstraction-Layer
 * @brief Drivers for the different CPU peripherals.
 */

/**
 * @defgroup Drivers Drivers-Layer
 * @brief Drivers for the different sensors and communication interfaces.
 */

/**
 * @defgroup Components Components-Layer
 * @brief Library for the application specific communication with sensors and interfaces.
 */

/**
 * @defgroup Application Application-Library
 * @brief Library for application specific functions.
 */

#include "Application/controller.h"
#include "Application/error_handler.h"
#include "Application/mode_handler.h"
#include "Components/actuators.h"
#include "Components/flightcomputer.h"
#include "Components/imu.h"
#include "Components/remote.h"
#include "Components/system.h"

/**
 * Frequency divider between the timer interrupt and sending new messages.
 * 6 * 16.384 approx 100ms
 */
enum { FLIGHTCOMPUTER_SEND_PERIOD = 6 };

/**
 * Offset from the remote data (in [0, 1000]) to the servo_motor_cmd data (in [-500, 500])
 */
enum { SERVO_REMOTE_OFFSET = 1000U / 2 };

void timer_tick(void) {
    imu_data_t imu_data;
    remote_data_t remote_data;
    flightcomputer_setpoint_t flightcomputer_setpoint;
    mode_handler_mode_t mode = mode_handler_handle(&imu_data, &remote_data, &flightcomputer_setpoint);

    /*
     * Calculate outputs
     */
    actuator_cmd_t actuator_cmd;
    switch (mode) {
        case MODE_FLIGHTCOMPUTER: {
            controller_result_t controller_result =
                    controller_update(&imu_data, flightcomputer_setpoint.roll, flightcomputer_setpoint.pitch);
            actuator_cmd.servo_left = controller_result.elevon_left;
            actuator_cmd.servo_right = controller_result.elevon_right;
            actuator_cmd.motor = flightcomputer_setpoint.motor;
            break;
        }
        case MODE_REMOTE:
            actuator_cmd.motor = remote_data.throttle_raw;
            actuator_cmd.servo_left = remote_data.elevon_left_mixed - SERVO_REMOTE_OFFSET;
            actuator_cmd.servo_right = remote_data.elevon_right_mixed - SERVO_REMOTE_OFFSET;
            break;
        case MODE_STABILISED_FAILSAVE: {
            controller_result_t controller_result = controller_update(&imu_data, 0, 0);
            actuator_cmd.servo_left = controller_result.elevon_left;
            actuator_cmd.servo_right = controller_result.elevon_right;
            actuator_cmd.motor = 0;
            break;
        }
        case MODE_FAILSAVE:
            actuator_cmd.motor = 0;
            actuator_cmd.servo_left = 0;
            actuator_cmd.servo_right = 0;
            break;
    }

    actuators_set(&actuator_cmd);

    /*
     * Send information to FCPS
     */
    static volatile uint8_t fcps_send_mux = 0;
    fcps_send_mux += 1;
    if (fcps_send_mux >= FLIGHTCOMPUTER_SEND_PERIOD) {
        flightcomputer_send(&imu_data, &remote_data, &actuator_cmd);
        fcps_send_mux = 0;
    }

    /*
     * Read the next IMU data
     */
    imu_start_sampling();
}

int main(void) {
    system_pre_init(timer_tick);
    error_handler_init();

    imu_init();
    remote_init();
    flightcomputer_init();
    actuators_init();

    imu_start_sampling();

    system_post_init();

    while (true) {
        system_reset_watchdog();
    }
}
