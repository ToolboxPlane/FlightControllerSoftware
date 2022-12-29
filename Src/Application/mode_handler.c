/**
 * @file mode_handler.c
 * @author Paul Nykiel
 * @date 21.12.22
 * @brief Implementation of the mode handler application.
 * @ingroup Application
 */
#include "mode_handler.h"

#include "error_handler.h"

enum {
    IMU_TIMEOUT = (uint16_t) (100 / 16.384),
    FLIGHTCOMPUTER_TIMOUT = (uint16_t) ((2 * 100) / 16.384),
    REMOTE_TIMEOUT = (uint16_t) (200 / 16.384),
};

static uint8_t imu_timeout_counter;
static uint8_t flightcomputer_timeout_counter;
static uint8_t remote_timeout_counter;

void mode_handler_init(void) {
    imu_timeout_counter = IMU_TIMEOUT;
    flightcomputer_timeout_counter = FLIGHTCOMPUTER_TIMOUT;
    remote_timeout_counter = REMOTE_TIMEOUT;
}

mode_handler_mode_t mode_handler_handle(imu_data_t *imu_data, remote_data_t *remote_data,
                                        flightcomputer_setpoint_t *flightcomputer_setpoint) {
    /*
     * Timeout Handling
     */
    if (!imu_data_available()) {
        imu_timeout_counter += 1;
    } else {
        imu_timeout_counter = 0;
    }
    bool imu_active = true;
    if (imu_timeout_counter >= IMU_TIMEOUT) {
        imu_timeout_counter = IMU_TIMEOUT;
        imu_active = false;
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
    }
    *flightcomputer_setpoint = flightcomputer_get_setpoint();

    /*
     * Set only as active with both active and ok
     */
    imu_active = imu_active && imu_data->imu_ok;
    remote_active = remote_active && remote_data->remote_ok;

    /*
     * Signal warning if not active
     */
    if (!flightcomputer_active) {
        error_handler_handle_warning(MODE_HANDLER, MODE_HANDLER_ERROR_NO_FCP_DATA);
    }
    if (!remote_active) {
        error_handler_handle_warning(MODE_HANDLER, MODE_HANDLER_ERROR_NO_REMOTE_DATA);
    }
    if (!imu_active) {
        error_handler_handle_warning(MODE_HANDLER, MODE_HANDLER_ERROR_NO_IMU_DATA);
    }


    /*
     * Setpoint selection
     */
    if (imu_active && remote_active && flightcomputer_active && !remote_data->override_active &&
        remote_data->is_armed) {
        return MODE_FLIGHTCOMPUTER;
    }

    if (remote_active) {
        return MODE_REMOTE;
    }

    if (imu_active) {
        return MODE_STABILISED_FAILSAVE;
    }

    return MODE_FAILSAVE;
}
