/**
 * @file bno055.h
 * @author paul
 * @date 01.11.22
 * Description here TODO
 */
#ifndef FLIGHTCONTROLLER_BNO055_H
#define FLIGHTCONTROLLER_BNO055_H

#include "bno055_uart.h"

typedef enum {
    system_idle = 0x0,
    system_error = 0x1,
    initializing_peripherals = 0x2,
    system_initialization = 0x3,
    executing_selftest = 0x4,
    sensor_fusion_algorithm_running = 0x5,
    system_running_without_fusion_algorithm = 0x6,
    unknown_status,
} bno055_status_t;

typedef enum {
    no_error = 0x0,
    peripheral_initialization_error = 0x1,
    system_initialization_error = 0x2,
    self_test_result_failed = 0x3,
    register_map_value_out_of_range = 0x4,
    register_map_address_out_of_range = 0x5,
    register_map_write_error = 0x6,
    bno_low_power_mode_not_available_for_selected_operation_mode = 0x7,
    accelerometer_power_mode_not_available = 0x8,
    fusion_algorithm_configuration_error = 0x9,
    sensor_configuration_error = 0xa,
    unknown_error
} bno055_error_t;

typedef enum {
    config_mode = 0b0000,
    acc_only = 0b0001,
    mag_only = 0b0010,
    gyro_only = 0b0011,
    acc_mag = 0b0100,
    acc_gyro = 0b0101,
    mag_gyro = 0b0110,
    amg = 0b0111,
    imu = 0b1000,
    compass = 0b1001,
    m4g = 0b1010,
    ndof_fmc_off = 0b1011,
    ndof = 0b1100
} bno055_opr_mode_t;

typedef enum { x_axis = 0, y_axis = 1, z_axis = 2 } bno055_axis_remap_axis_t;

typedef enum { positive = 0, neg = 1 } bno055_axis_remap_sign_t;

typedef enum { mps2 = 0, mg = 1 } bno055_unit_sel_acc;

typedef enum { dps = 0, rps = 1 } bno055_unit_sel_angular_rate;

typedef enum { degrees = 0, radians = 1 } bno055_unit_sel_euler_angles;

typedef enum { celsius = 0, fahrenheit = 1 } bno055_unit_sel_temperature;

typedef enum { windows = 0, android = 1 } bno055_unit_sel_orientation_def;

void bno055_init(void);

void bno055_write_reset(bno_callback_t callback);
void bno055_read_system_status(bno055_status_t *out, bno_callback_t callback);
void bno055_read_system_error(bno055_error_t *out, bno_callback_t callback);
void bno055_read_calib_status(uint8_t *out, bno_callback_t callback);
void bno055_write_opr_mode(bno055_opr_mode_t op_mode, bno_callback_t callback);
// clang-format off
void bno055_write_unit_selection(bno055_unit_sel_acc acc_unit, bno055_unit_sel_angular_rate angular_rate_unit,bno055_unit_sel_euler_angles euler_angles_unit,bno055_unit_sel_temperature temperature_unit,bno055_unit_sel_orientation_def orientation_def, bno_callback_t callback);
void bno055_write_remap_axis(bno055_axis_remap_axis_t new_x, bno055_axis_remap_axis_t new_y, bno055_axis_remap_axis_t new_z,bno_callback_t callback);
void bno055_write_remap_axis_sign(bno055_axis_remap_sign_t new_x_sign, bno055_axis_remap_sign_t new_y_sign,bno055_axis_remap_sign_t new_z_sign, bno_callback_t callback);
// clang-format on
void bno055_read_self_test(uint8_t *out, bno_callback_t callback);


void bno055_read_acc_xyz_mul_100(int16_t *out, bno_callback_t callback);

void bno055_read_gyr_xyz_mul_16(int16_t *out, bno_callback_t callback);

void bno055_read_eul_xyz_2_mul_16(int16_t *out, bno_callback_t callback);

#endif // FLIGHTCONTROLLER_BNO055_H
