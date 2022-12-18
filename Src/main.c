/**
 * @file main.c
 * @author Paul Nykiel
 * @date 12.04.19
 * Description here TODO
 */
#include "Application/error_handler.h"
#include "Components/flightcomputer.h"
#include "Components/imu.h"
#include "Components/remote.h"
#include "Components/servo_motor.h"
#include "Components/system.h"

enum {
    APPLICATION_ERROR_NO_IMU_DATA = 1,
    APPLICATION_ERROR_NO_FCP_DATA = 2,
    APPLICATION_ERROR_NO_REMOTE_DATA = 3,
};

typedef enum { SPS_FLIGHTCOMPUTER, SPS_REMOTE, SPS_STABILISED_FAILSAVE, SPS_FAILSAVE } setpoint_source_t;

enum {
    IMU_TIMEOUT = 6,            // 6 * 16.384 \approx 100ms
    FLIGHTCOMPUTER_TIMOUT = 12, // 12 * 16.384 \approx 200ms
    REMOTE_TIMEOUT = 12         // 12 * 16.384 \approx 200ms
};

void timer_tick(void) {
    /*
     * Timeout Handling
     */
    static uint8_t imu_timeout_counter = 0;
    static uint8_t flightcomputer_timeout_counter = 0;
    static uint8_t remote_timeout_counter = 0;

    if (!imu_data_available()) {
        imu_timeout_counter += 1;
    } else {
        imu_timeout_counter = 0;
    }
    bool imu_active = true;
    if (imu_timeout_counter >= IMU_TIMEOUT) {
        imu_timeout_counter = IMU_TIMEOUT;
        imu_active = false;
        // error_handler_handle_warning(APPLICATION, APPLICATION_ERROR_NO_IMU_DATA);
    }
    imu_data_t imu_data = imu_get_latest_data();

    if (!remote_data_available()) {
        remote_timeout_counter += 1;
    } else {
        remote_timeout_counter = 0;
    }
    bool remote_active = true;
    if (remote_timeout_counter >= REMOTE_TIMEOUT) {
        remote_timeout_counter = REMOTE_TIMEOUT;
        remote_active = false;
        // error_handler_handle_warning(APPLICATION, APPLICATION_ERROR_NO_REMOTE_DATA);
    }
    remote_data_t remote_data = remote_get_data();

    if (!flightcomputer_setpoint_available()) {
        flightcomputer_timeout_counter += 1;
    } else {
        flightcomputer_timeout_counter = 0;
    }
    bool flightcomputer_active = true;
    if (flightcomputer_timeout_counter >= FLIGHTCOMPUTER_TIMOUT) {
        flightcomputer_timeout_counter = FLIGHTCOMPUTER_TIMOUT;
        flightcomputer_active = false;
        // error_handler_handle_warning(APPLICATION, APPLICATION_ERROR_NO_FCP_DATA);
    }
    flightcomputer_setpoint_t flightcomputer_setpoint = flightcomputer_get_setpoint();

    /*
     * Setpoint selection
     */
    setpoint_source_t setpoint_source = SPS_FAILSAVE;
    if (!imu_active && !remote_active) { // No IMU and no remote
        setpoint_source = SPS_FAILSAVE;
    } else if (!imu_active || !flightcomputer_active) { // Remote but no IMU or no FCP
        setpoint_source = SPS_REMOTE;
    } else if (!remote_active) { // IMU but no remote
        setpoint_source = SPS_STABILISED_FAILSAVE;
    } else { // IMU, remote and FCP are active
        if (remote_data.override_active) {
            setpoint_source = SPS_REMOTE;
        } else {
            setpoint_source = SPS_FLIGHTCOMPUTER;
        }
    }

    /*
     * Calculate outputs
     */
    servo_motor_cmd_t servo_motor_cmd;
    switch (setpoint_source) {
        case SPS_FLIGHTCOMPUTER:
            servo_motor_cmd.motor = flightcomputer_setpoint.motor;
            // TODO controller
            break;
        case SPS_REMOTE:
            servo_motor_cmd.motor = remote_data.throttle_raw;
            servo_motor_cmd.servo_left = remote_data.elevon_left_mixed;
            servo_motor_cmd.servo_right = remote_data.elevon_right_mixed;
            break;
        case SPS_STABILISED_FAILSAVE:
            servo_motor_cmd.motor = 0;
            // TODO controller
            break;
        case SPS_FAILSAVE:
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
    if (fcps_send_mux >= 6) {
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

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
    while (true) {
        system_reset_watchdog();
    }
#pragma clang diagnostic pop
}
