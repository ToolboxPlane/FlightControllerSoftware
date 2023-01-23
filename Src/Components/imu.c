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
    INIT,
    EUL,
    GYR,
    ACC,
    STATUS,
    CALIB_STAT,
} bno_sampling_state_t;

// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables) only read if callback ready, behaviour checked
static volatile bno055_response_t response = write_fail;
// NOLINTNEXTLINE(cppcoreguidelines-avoid-non-const-global-variables) clear interaction in init, always properly set
static volatile bool callback_ready = false;

static volatile imu_data_t imu_datas[2];

static volatile bool sampling_complete = false;

static volatile bno_sampling_state_t current_sample_state;
static volatile uint8_t current_sample_state_id;


static void bno_callback(bno055_response_t response_) {
    response = response_;
    callback_ready = true;
}

void imu_init(void) {
    // Initialize physical connection
    bno055_init();

    // Set to config mode
    callback_ready = false;
    bno055_write_opr_mode(BNO055_OPR_MODE_CONFIG_MODE, bno_callback);
    _delay_ms(INIT_RESPONSE_TIMEOUT_MS);
    if (!callback_ready) {
        error_handler_handle_error(ERROR_HANDLER_GROUP_IMU, IMU_ERROR_INIT_TIMEOUT);
        return;
    }
    if (response != write_success) {
        error_handler_handle_error(ERROR_HANDLER_GROUP_BNO055, response + 1);
        return;
    }

    // Run Self Test
    callback_ready = false;
    bno055_self_test_result_t self_test_result;
    bno055_read_self_test(&self_test_result, bno_callback);
    _delay_ms(INIT_RESPONSE_TIMEOUT_MS);
    if (!callback_ready) {
        error_handler_handle_error(ERROR_HANDLER_GROUP_IMU, IMU_ERROR_INIT_TIMEOUT);
        return;
    }
    if (response != read_success) {
        error_handler_handle_error(ERROR_HANDLER_GROUP_BNO055, response + 1);
        return;
    }
    if (!self_test_result.acc_passed || !self_test_result.gyr_passed || !self_test_result.mag_passed ||
        !self_test_result.mcu_passed) {
        error_handler_handle_error(ERROR_HANDLER_GROUP_IMU, IMU_ERROR_INIT_SELF_TEST);
        return;
    }

    // Set unit selection
    callback_ready = false;
    bno055_write_unit_selection(BNO055_UNIT_SEL_ACC_MPS2, BNO055_UNIT_SEL_ANGULAR_RATE_DPS,
                                BNO055_UNIT_SEL_EULER_ANGLES_DEGREES, BNO055_UNIT_SEL_TEMPERATURE_CELSIUS,
                                BNO055_UNIT_SEL_ORIENTATION_DEF_WINDOWS, bno_callback);
    _delay_ms(INIT_RESPONSE_TIMEOUT_MS);
    if (!callback_ready) {
        error_handler_handle_error(ERROR_HANDLER_GROUP_IMU, IMU_ERROR_INIT_TIMEOUT);
        return;
    }
    if (response != write_success) {
        error_handler_handle_error(ERROR_HANDLER_GROUP_BNO055, response + 1);
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
    bno055_write_remap_axis(BNO055_AXIS_REMAP_Y_AXIS, BNO055_AXIS_REMAP_X_AXIS, BNO055_AXIS_REMAP_Z_AXIS, bno_callback);
    _delay_ms(INIT_RESPONSE_TIMEOUT_MS);
    if (!callback_ready) {
        error_handler_handle_error(ERROR_HANDLER_GROUP_IMU, IMU_ERROR_INIT_TIMEOUT);
        return;
    }
    if (response != write_success) {
        error_handler_handle_error(ERROR_HANDLER_GROUP_BNO055, response + 1);
        return;
    }

    callback_ready = false;
    bno055_write_remap_axis_sign(BNO055_AXIS_REMAP_SIGN_POSITIVE, BNO055_AXIS_REMAP_SIGN_POSITIVE,
                                 BNO055_AXIS_REMAP_SIGN_NEGATIVE, bno_callback);
    _delay_ms(INIT_RESPONSE_TIMEOUT_MS);
    if (!callback_ready) {
        error_handler_handle_error(ERROR_HANDLER_GROUP_IMU, IMU_ERROR_INIT_TIMEOUT);
        return;
    }
    if (response != write_success) {
        error_handler_handle_error(ERROR_HANDLER_GROUP_BNO055, response + 1);
        return;
    }

    // Set to NDOF-FMC-OFF
    callback_ready = false;
    bno055_write_opr_mode(BNO055_OPR_MODE_NDOF_FMC_OFF, bno_callback);
    _delay_ms(INIT_RESPONSE_TIMEOUT_MS);
    if (!callback_ready) {
        error_handler_handle_error(ERROR_HANDLER_GROUP_IMU, IMU_ERROR_INIT_TIMEOUT);
        return;
    }
    if (response != write_success) {
        error_handler_handle_error(ERROR_HANDLER_GROUP_BNO055, response + 1);
        return;
    }

    // Preparation for sampling
    imu_datas[0].imu_ok = false;
    imu_datas[1].imu_ok = false;
    sampling_complete = false;

    current_sample_state = INIT;
    response = read_success;
    callback_ready = true;
}

void imu_start_sampling(void) {
    static bno055_calib_status_t calib_status;
    static bno055_status_t bno_status;

    imu_data_t *imu_data = (imu_data_t *) (&imu_datas[current_sample_state_id]);

    if (callback_ready) {
        callback_ready = false;
        if (response != read_success) {
            if (response != bus_over_run_error) {
                error_handler_handle_warning(ERROR_HANDLER_GROUP_BNO055, response + 1);
            }

            switch (current_sample_state) {
                case EUL:
                    bno055_read_eul_xyz_2_mul_16(&imu_data->heading_mul_16, bno_callback);
                    break;
                case GYR:
                    bno055_read_gyr_xyz_mul_16(&imu_data->d_heading_mul_16, bno_callback);
                    break;
                case ACC:
                    bno055_read_acc_xyz_mul_100(&imu_data->acc_x_mul_100, bno_callback);
                    break;
                case STATUS:
                    bno055_read_system_status(&bno_status, bno_callback);
                    break;
                case CALIB_STAT:
                    bno055_read_calib_status(&calib_status, bno_callback);
                    break;
                case INIT:
                    // Shouldn't be here...
                    break;
            }
        } else {
            switch (current_sample_state) {
                case INIT:
                    current_sample_state = EUL;
                    bno055_read_eul_xyz_2_mul_16(&imu_data->heading_mul_16, bno_callback);
                    break;
                case EUL:
                    current_sample_state = GYR;
                    bno055_read_gyr_xyz_mul_16(&imu_data->d_heading_mul_16, bno_callback);
                    break;
                case GYR:
                    current_sample_state = ACC;
                    bno055_read_acc_xyz_mul_100(&imu_data->acc_x_mul_100, bno_callback);
                    break;
                case ACC:
                    current_sample_state = STATUS;
                    bno055_read_system_status(&bno_status, bno_callback);
                    break;
                case STATUS:
                    current_sample_state = CALIB_STAT;
                    bno055_read_calib_status(&calib_status, bno_callback);
                    break;
                case CALIB_STAT:
                    // Reduction of flags to imu_ok
                    if (bno_status != BNO055_STATUS_SENSOR_FUSION_ALGORITHM_RUNNING) {
                        //error_handler_handle_warning(ERROR_HANDLER_GROUP_IMU, IMU_ERROR_STATUS);
                        imu_data->imu_ok = false;
                    } else if (calib_status.sys_status < SYS_CALIB_TRESH || calib_status.gyr_status < GYR_CALIB_TRESH ||
                               calib_status.acc_status < ACC_CALIB_TRESH || calib_status.mag_status < MAG_CALIB_TRESH) {
                        imu_data->imu_ok = false;
                    } else {
                        imu_data->imu_ok = true;
                    }

                    // Signal completed sampling
                    current_sample_state_id = 1 - current_sample_state_id;
                    sampling_complete = true;
                    imu_data = (imu_data_t *) (&imu_datas[current_sample_state_id]);

                    current_sample_state = EUL;
                    bno055_read_eul_xyz_2_mul_16(&imu_data->heading_mul_16, bno_callback);
                    break;
            }
        }
    } else {
        error_handler_handle_warning(ERROR_HANDLER_GROUP_IMU, IMU_ERROR_READ_TIMEOUT);
    }
}

imu_data_t imu_get_latest_data(void) {
    sampling_complete = false;
    return imu_datas[1 - current_sample_state_id];
}

bool imu_data_available(void) {
    return sampling_complete;
}
