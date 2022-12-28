/**
 * @file imu.c
 * @author Paul Nykiel
 * @date 18.04.19
 * @brief Declaration of the IMU interface component.
 * @ingroup Components
 */

#include "imu.h"

#include <Application/error_handler.h>
#include <Drivers/bno055.h>
#include <util/delay.h>

enum { INIT_RESPONSE_TIMEOUT_MS = 20 };
enum { ACC_CALIB_TRESH = 2 };
enum { MAG_CALIB_TRESH = 2 };
enum { GYR_CALIB_TRESH = 2 };
enum { SYS_CALIB_TRESH = 3 };

/**
 * State of the internal sampling state machine.
 */
typedef enum {
    EUL,
    GYR,
    ACC,
    STATUS,
    CALIB_STAT,
} bno_sampling_state_t;

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables) only read if callback ready, behaviour checked
static volatile bno055_response_t init_response = write_fail;
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables) clear interaction in init, always properly set
static volatile bool callback_ready = false;

static volatile bno_sampling_state_t bno_sampling_state = EUL;
static volatile imu_data_t imu_datas[2];
static bno055_calib_status_t calib_status;
static bno055_status_t bno_status;
static volatile uint8_t current_sample_state_id = 0;
static volatile bool sampling_complete = false;

static void bno_init_callback(bno055_response_t response) {
    init_response = response;
    callback_ready = true;
}

static void bno_sample_callback(bno055_response_t response) {
    imu_data_t *imu_data = (imu_data_t *) (&imu_datas[current_sample_state_id]);
    if (response == read_success) {
        switch (bno_sampling_state) {
            case EUL:
                bno_sampling_state = GYR;
                // imu_start_sampling();
                break;
            case GYR:
                bno_sampling_state = ACC;
                // imu_start_sampling();
                break;
            case ACC:
                bno_sampling_state = STATUS;
                // imu_start_sampling();
                break;
            case STATUS:
                if (bno_status != sensor_fusion_algorithm_running) {
                    error_handler_handle_warning(IMU, IMU_ERROR_STATUS);
                    imu_data->imu_ok = false;
                } else {
                    imu_data->imu_ok = true;
                }
                bno_sampling_state = CALIB_STAT;
                // imu_start_sampling();
                break;
            case CALIB_STAT:
                current_sample_state_id = 1 - current_sample_state_id;
                if (calib_status.sys_status < SYS_CALIB_TRESH || calib_status.gyr_status < GYR_CALIB_TRESH ||
                    calib_status.acc_status < ACC_CALIB_TRESH || calib_status.mag_status < MAG_CALIB_TRESH) {
                    imu_data->imu_ok = false;
                }
                sampling_complete = true;
                bno_sampling_state = EUL;
                break;
        }
    } else if (response != bus_over_run_error) { // Bus overrun just happens...
        error_handler_handle_warning(BNO055, response + 1);
        imu_data->imu_ok = false;
        current_sample_state_id = 1 - current_sample_state_id;
        bno_sampling_state = EUL;
    }
}

void imu_init(void) {
    imu_datas[0].imu_ok = false;
    imu_datas[1].imu_ok = false;

    current_sample_state_id = 0;

    bno055_init();

    // Set to config mode
    callback_ready = false;
    bno055_write_opr_mode(config_mode, bno_init_callback);
    _delay_ms(INIT_RESPONSE_TIMEOUT_MS);
    if (!callback_ready) {
        error_handler_handle_error(IMU, IMU_ERROR_INIT_TIMEOUT);
        return;
    }
    if (init_response != write_success) {
        error_handler_handle_error(BNO055, init_response + 1);
        return;
    }

    // Run Self Test
    callback_ready = false;
    bno055_self_test_result_t self_test_result;
    bno055_read_self_test(&self_test_result, bno_init_callback);
    _delay_ms(INIT_RESPONSE_TIMEOUT_MS);
    if (!callback_ready) {
        error_handler_handle_error(IMU, IMU_ERROR_INIT_TIMEOUT);
        return;
    }
    if (init_response != read_success) {
        error_handler_handle_error(BNO055, init_response + 1);
        return;
    }
    if (!self_test_result.acc_passed || !self_test_result.gyr_passed || !self_test_result.mag_passed ||
        !self_test_result.mcu_passed) {
        error_handler_handle_error(IMU, IMU_ERROR_INIT_SELF_TEST);
        return;
    }

    // Set unit selection
    callback_ready = false;
    bno055_write_unit_selection(mps2, dps, degrees, celsius, windows, bno_init_callback);
    _delay_ms(INIT_RESPONSE_TIMEOUT_MS);
    if (!callback_ready) {
        error_handler_handle_error(IMU, IMU_ERROR_INIT_TIMEOUT);
        return;
    }
    if (init_response != write_success) {
        error_handler_handle_error(BNO055, init_response + 1);
        return;
    }

    /*
     * Reconfigure Axis:
     *
     * Pin 1 is on front right (opposite to the XT30 connector on the board).
     *
     * Thus, the IMU axis correspond to the following plane axis:
     *  - IMU X: Positive Pitch (Y)
     *  - IMU Y: Positive Roll (X)
     *  - IMU Z: Negative Yaw (-Z)
     */
    callback_ready = false;
    bno055_write_remap_axis(y_axis, x_axis, z_axis, bno_init_callback);
    _delay_ms(INIT_RESPONSE_TIMEOUT_MS);
    if (!callback_ready) {
        error_handler_handle_error(IMU, IMU_ERROR_INIT_TIMEOUT);
        return;
    }
    if (init_response != write_success) {
        error_handler_handle_error(BNO055, init_response + 1);
        return;
    }

    callback_ready = false;
    bno055_write_remap_axis_sign(positive, positive, neg, bno_init_callback);
    _delay_ms(INIT_RESPONSE_TIMEOUT_MS);
    if (!callback_ready) {
        error_handler_handle_error(IMU, IMU_ERROR_INIT_TIMEOUT);
        return;
    }
    if (init_response != write_success) {
        error_handler_handle_error(BNO055, init_response + 1);
        return;
    }

    // Set to NDOF-FMC-OFF
    callback_ready = false;
    bno055_write_opr_mode(ndof_fmc_off, bno_init_callback);
    _delay_ms(INIT_RESPONSE_TIMEOUT_MS);
    if (!callback_ready) {
        error_handler_handle_error(IMU, IMU_ERROR_INIT_TIMEOUT);
        return;
    }
    if (init_response != write_success) {
        error_handler_handle_error(BNO055, init_response + 1);
        return;
    }

    bno_sampling_state = EUL;
}

void imu_start_sampling(void) {
    imu_data_t *imu_data = (imu_data_t *) (&imu_datas[current_sample_state_id]);
    switch (bno_sampling_state) {
        case EUL:
            bno055_read_eul_xyz_2_mul_16(&imu_data->heading_mul_16, bno_sample_callback);
            break;
        case GYR:
            bno055_read_gyr_xyz_mul_16(&imu_data->d_heading_mul_16, bno_sample_callback);
            break;
        case ACC:
            bno055_read_acc_xyz_mul_100(&imu_data->acc_x_mul_100, bno_sample_callback);
            break;
        case STATUS:
            bno055_read_system_status(&bno_status, bno_sample_callback);
            break;
        case CALIB_STAT:
            bno055_read_calib_status(&calib_status, bno_sample_callback);
            break;
    }
}

imu_data_t imu_get_latest_data(void) {
    sampling_complete = false;
    return imu_datas[1 - current_sample_state_id];
}

bool imu_data_available(void) {
    return sampling_complete;
}
