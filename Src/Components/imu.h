/**
 * @file imu.h
 * @author Paul Nykiel
 * @date 18.04.19
 * @brief Implementation of the IMU interface component.
 * @ingroup Components
 */

#ifndef FLIGHTCONTROLLER_IMU_H
#define FLIGHTCONTROLLER_IMU_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    int16_t heading_mul_16, pitch_mul_16, roll_mul_16;
    int16_t d_heading_mul_16, d_pitch_mul_16, d_roll_mul_16;
    int16_t acc_x_mul_100, acc_y_mul_100, acc_z_mul_100;
    bool imu_ok;
} imu_data_t;

typedef enum {
    IMU_ERROR_INIT_CONFIG_MODE = 1,
    IMU_ERROR_INIT_SELF_TEST_WRITE = 2,
    IMU_ERROR_INIT_SELF_TEST = 3,
    IMU_ERROR_INIT_UNIT_SEL = 4,
    IMU_ERROR_INIT_REMAP_AXIS = 5,
    IMU_ERROR_INIT_REMAP_AXIS_SIGN = 6,
    IMU_ERROR_INIT_NDOF_FMC_OFF = 7,
    IMU_ERROR_STATUS = 8,
    IMU_ERROR_UART = 9,
} imu_error_t;

void imu_init(void);
void imu_start_sampling(void);
imu_data_t imu_get_latest_data(void);
bool imu_data_available(void);

#endif // FLIGHTCONTROLLER_IMU_H
