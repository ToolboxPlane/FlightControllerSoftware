/**
 * @file mode_handler.c
 * @author Paul Nykiel
 * @date 21.12.22
 * @brief Implementation of the mode handler application.
 */
#include "mode_handler.h"

#include "error_handler.h"

enum {
    IMU_TIMEOUT = 6,            // 6 * 16.384 \approx 100ms
    FLIGHTCOMPUTER_TIMOUT = 12, // 12 * 16.384 \approx 200ms
    REMOTE_TIMEOUT = 12,        // 12 * 16.384 \approx 200ms
};

mode_handler_mode_t mode_handler_handle(imu_data_t *imu_data, remote_data_t *remote_data,
                                        flightcomputer_setpoint_t *flightcomputer_setpoint) {
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
        error_handler_handle_warning(MODE_HANDLER, MODE_HANDLER_ERROR_NO_IMU_DATA);
    }
    *imu_data = imu_get_latest_data();

    if (!remote_data_available()) {
        remote_timeout_counter += 1;
    } else {
        remote_timeout_counter = 0;
    }
    bool remote_active = true;
    if (remote_timeout_counter >= REMOTE_TIMEOUT) {
        remote_timeout_counter = REMOTE_TIMEOUT;
        remote_active = false;
        error_handler_handle_warning(MODE_HANDLER, MODE_HANDLER_ERROR_NO_REMOTE_DATA);
    }
    *remote_data = remote_get_data();

    if (!flightcomputer_setpoint_available()) {
        flightcomputer_timeout_counter += 1;
    } else {
        flightcomputer_timeout_counter = 0;
    }
    bool flightcomputer_active = true;
    if (flightcomputer_timeout_counter >= FLIGHTCOMPUTER_TIMOUT) {
        flightcomputer_timeout_counter = FLIGHTCOMPUTER_TIMOUT;
        flightcomputer_active = false;
        error_handler_handle_warning(MODE_HANDLER, MODE_HANDLER_ERROR_NO_FCP_DATA);
    }
    *flightcomputer_setpoint = flightcomputer_get_setpoint();

    /*
     * Set only as active with both active and ok
     */
    imu_active = imu_active && imu_data->imu_ok;
    remote_active = remote_active && remote_data->remote_ok;

    /*
     * Setpoint selection
     */
    if (imu_active && remote_active && flightcomputer_active) {
        if (!remote_data->override_active && remote_data->is_armed) {
            return MODE_FLIGHTCOMPUTER;
        }

        if (remote_data->override_active) {
            return MODE_REMOTE;
        }
    }

    if (!flightcomputer_active && remote_active) {
        return MODE_REMOTE;
    }

    if (imu_active) {
        return MODE_STABILISED_FAILSAVE;
    }

    return MODE_FAILSAVE;
}
