/**
 * @file imu.h
 * @author paul
 * @date 18.04.19
 * @brief input @TODO
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

bool imu_init(void);
void imu_start_sampling(void);
imu_data_t imu_get_latest_data(void);
bool imu_data_available(void);

#endif // FLIGHTCONTROLLER_IMU_H
