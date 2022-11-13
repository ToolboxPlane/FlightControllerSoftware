/**
 * @file input.c
 * @author paul
 * @date 18.04.19
 * @brief input @TODO
 */

#include "imu_handler.h"

#include <util/delay.h>

#include "../Drivers/bno055.h"

typedef enum {
    ROLL,
    PITCH,
    HEADING,
    D_ROLL,
    D_PITCH,
    D_HEADING,
    ACC_X,
    ACC_Y,
    ACC_Z,
    STATUS,
    ERROR_REG,
    CALIB_STAT,
    SAMPLING_ERROR
} bno_sampling_state_t;

static volatile bno055_response_t init_response;
static volatile bool callback_ready;
static volatile bno_sampling_state_t bno_sampling_state;

static volatile imu_data_t imu_datas[2];
static volatile uint8_t current_sample_state_id = 0;

void bno_init_callback(bno055_response_t response) {
    init_response = response;
    callback_ready = true;
}

void bno_sample_callback(bno055_response_t response) {
    static bno055_status_t bno_status;
    static bno055_error_t bno_erro;
    static uint8_t calib_status;

    imu_data_t *imu_data = (imu_data_t *) (&imu_datas[current_sample_state_id]);
    if (response == read_success) {
        switch (bno_sampling_state) {
            case ROLL:
                bno_sampling_state = PITCH;
                bno055_read_eul_y_2_mul_16(&imu_data->pitch_mul_16, bno_sample_callback);
                break;
            case PITCH:
                bno_sampling_state = HEADING;
                bno055_read_eul_z_2_mul_16(&imu_data->heading_mul_16, bno_sample_callback);
                break;
            case HEADING:
                bno_sampling_state = D_ROLL;
                bno055_read_gyr_x_mul_16(&imu_data->d_roll_mul_16, bno_sample_callback);
                break;
            case D_ROLL:
                bno_sampling_state = D_PITCH;
                bno055_read_gyr_y_mul_16(&imu_data->d_pitch, bno_sample_callback);
                break;
            case D_PITCH:
                bno_sampling_state = D_HEADING;
                bno055_read_gyr_z_mul_16(&imu_data->d_heading_mul_16, bno_sample_callback);
                break;
            case D_HEADING:
                bno_sampling_state = ACC_X;
                bno055_read_acc_x_mul_100(&imu_data->acc_x_mul_100, bno_sample_callback);
                break;
            case ACC_X:
                bno_sampling_state = ACC_Y;
                bno055_read_acc_y_mul_100(&imu_data->acc_y_mul_100, bno_sample_callback);
                break;
            case ACC_Y:
                bno_sampling_state = ACC_Z;
                bno055_read_acc_z_mul_100(&imu_data->acc_z_mul_100, bno_sample_callback);
                break;
            case ACC_Z:
                bno_sampling_state = STATUS;
                bno055_read_system_status(&bno_status, bno_sample_callback);
                break;
            case STATUS:
                if (bno_status != sensor_fusion_algorithm_running) {
                    imu_data->imu_ok = false;
                    bno_sampling_state = ERROR_REG;
                    bno055_read_system_error(&bno_erro, bno_sample_callback);
                } else {
                    imu_data->imu_ok = true;
                    bno_sampling_state = CALIB_STAT;
                    bno055_read_calib_status(&calib_status, bno_sample_callback);
                }
                break;
            case ERROR_REG:
                bno_sampling_state = CALIB_STAT;
                bno055_read_calib_status(&calib_status, bno_sample_callback);
                break;
            case CALIB_STAT:
                current_sample_state_id = 1 - current_sample_state_id;
                imu_handler_start_sampling();
                break;
            default:
                bno_sampling_state = SAMPLING_ERROR;
                break;
        }
    } else {
        bno_sampling_state = SAMPLING_ERROR;
    }
}

bool imu_handler_init(void) {
    // TODO run self test, check CHIP IDs, reconfigure axis
    bno055_init();

    // Set to config mode
    callback_ready = false;
    bno055_write_opr_mode(config_mode, bno_init_callback);
    _delay_ms(20);
    if (!callback_ready || init_response != write_success) {
        return false;
    }

    // Check BNO-ID (todo others)

    // Run Self Test

    // Set unit selection
    callback_ready = false;
    bno055_write_unit_selection(mps2, dps, degrees, celsius, windows, bno_init_callback);
    _delay_ms(20);
    if (!callback_ready || init_response != write_success) {
        return false;
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
    _delay_ms(20);
    if (!callback_ready || init_response != write_success) {
        return false;
    }

    callback_ready = false;
    bno055_write_remap_axis_sign(positive, positive, neg, bno_init_callback);
    _delay_ms(20);
    if (!callback_ready || init_response != write_success) {
        return false;
    }

    // Set to NDOF-FMC-OFF
    callback_ready = false;
    bno055_write_opr_mode(ndof_fmc_off, bno_init_callback);
    _delay_ms(20);
    if (!callback_ready || init_response != write_success) {
        return false;
    }

    current_sample_state_id = 0;

    return true;
}

void imu_handler_start_sampling(void) {
    bno_sampling_state = ROLL;
    imu_data_t *imu_data = (imu_data_t *) (&imu_datas[current_sample_state_id]);
    bno055_read_eul_x_2_mul_16(&imu_data->roll_mul_16, bno_sample_callback);
}

imu_data_t imu_handler_get_latest_data(void) {
    return imu_datas[1 - current_sample_state_id];
}
