/**
 * @file main.c
 * @author Paul Nykiel
 * @date 12.04.19
 * @brief Main file of the Flightcontroller firmware.
 */
#include "Application/controller.h"
#include "Application/error_handler.h"
#include "Application/mode_handler.h"
#include "Components/flightcomputer.h"
#include "Components/imu.h"
#include "Components/remote.h"
#include "Components/servo_motor.h"
#include "Components/system.h"

enum {
    FLIGHTCOMPUTER_SEND_PERIOD = 6, // 6 * 16.384 \approx 100ms
};

void timer_tick(void) {
    imu_data_t imu_data;
    remote_data_t remote_data;
    flightcomputer_setpoint_t flightcomputer_setpoint;
    mode_handler_mode_t mode = mode_handler_handle(&imu_data, &remote_data, &flightcomputer_setpoint);

    /*
     * Calculate outputs
     */
    servo_motor_cmd_t servo_motor_cmd;
    switch (mode) {
        case MODE_FLIGHTCOMPUTER: {
            controller_result_t controller_result =
                    controller_update(&imu_data, flightcomputer_setpoint.roll, flightcomputer_setpoint.pitch);
            servo_motor_cmd.servo_left = controller_result.elevon_left;
            servo_motor_cmd.servo_right = controller_result.elevon_right;
            servo_motor_cmd.motor = flightcomputer_setpoint.motor;
            break;
        }
        case MODE_REMOTE:
            servo_motor_cmd.motor = remote_data.throttle_raw;
            servo_motor_cmd.servo_left = remote_data.elevon_left_mixed;
            servo_motor_cmd.servo_right = remote_data.elevon_right_mixed;
            break;
        case MODE_STABILISED_FAILSAVE: {
            controller_result_t controller_result = controller_update(&imu_data, 0, 0);
            servo_motor_cmd.servo_left = controller_result.elevon_left;
            servo_motor_cmd.servo_right = controller_result.elevon_right;
            servo_motor_cmd.motor = 0;
            break;
        }
        case MODE_FAILSAVE:
            servo_motor_cmd.motor = 0;
            servo_motor_cmd.servo_left = 0;
            servo_motor_cmd.servo_right = 0;
            break;
    }

    servo_motor_set(&servo_motor_cmd);

    /*
     * Send information to FCPS
     */
    static volatile uint8_t fcps_send_mux = 0;
    fcps_send_mux += 1;
    if (fcps_send_mux >= FLIGHTCOMPUTER_SEND_PERIOD) {
        flightcomputer_send(&imu_data, &remote_data, &servo_motor_cmd);
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
    servo_motor_init();

    imu_start_sampling();

    system_post_init();

    while (true) {
        system_reset_watchdog();
    }
}
