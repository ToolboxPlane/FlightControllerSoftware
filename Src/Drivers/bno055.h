/**
 * @file bno055.h
 * @author Paul Nykiel
 * @date 01.11.22
 * @brief Definition of the interface independent library functions for the Bosch Sensortec BNO-055.
 * @ingroup Drivers
 */
#ifndef FLIGHTCONTROLLER_BNO055_H
#define FLIGHTCONTROLLER_BNO055_H

#include <stdbool.h>

#include "bno055_uart.h"

/**
 * Possible values of the status register.
 */
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

/**
 * Possible values of the error register.
 */
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

/**
 * Possible operation modes of the sensor.
 */
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

/**
 * Possible axis names used for remapping.
 */
typedef enum { x_axis = 0, y_axis = 1, z_axis = 2 } bno055_axis_remap_axis_t;

/**
 * Possible axis signs used for sign remapping.
 */
typedef enum { positive = 0, neg = 1 } bno055_axis_remap_sign_t;

/**
 * Possible acceleration units for the unit selection.
 */
typedef enum { mps2 = 0, mg = 1 } bno055_unit_sel_acc;

/**
 * Possible angular rate units for the unit selection.
 */
typedef enum { dps = 0, rps = 1 } bno055_unit_sel_angular_rate;

/**
 * Possible euler angle units for the unit selection.
 */
typedef enum { degrees = 0, radians = 1 } bno055_unit_sel_euler_angles;

/**
 * Possible temperature units for the unit selection.
 */
typedef enum { celsius = 0, fahrenheit = 1 } bno055_unit_sel_temperature;

/**
 * Possible axis-frame options for the unit selection.
 */
typedef enum { windows = 0, android = 1 } bno055_unit_sel_orientation_def;

/**
 * Information in the calibration status register.
 */
typedef struct {
    unsigned mag_status : 2; ///< Status of the magnetometer.
    unsigned acc_status : 2; ///< Status of the accelerometer.
    unsigned gyr_status : 2; ///< Status of the gyro.
    unsigned sys_status : 2; ///< Status of the system.
} bno055_calib_status_t;

/**
 * Information in the self-test register.
 */
typedef struct {
    bool acc_passed : 1; ///< True if accelerometer passed self test.
    bool mag_passed : 1; ///< True if magnetometer passed self test.
    bool gyr_passed : 1; ///< True if the gyro passed self test.
    bool mcu_passed : 1; ///< True if the bno055-microcontroller passed self test.
} bno055_self_test_result_t;

/**
 * Initialize the physical connection to the BNO, this will not initialize the sensor itself.
 */
void bno055_init(void);

/**
 * Reset the sensor by writing a 1 to the respective bit in the system trigger register.
 * @param callback function to be called when the chip sends a response
 */
void bno055_write_reset(bno055_callback_t callback);

/**
 * Read the system status register of the sensor.
 * @param out out-parameter used for storing the result
 * @param callback callback that is called once the transaction is complete and "out" got set
 */
void bno055_read_system_status(bno055_status_t *out, bno055_callback_t callback);

/**
 * Read the system error register of the sensor.
 * @param out out-parameter used for storing the result
 * @param callback callback that is called once the transaction is complete and "out" got set
 */
void bno055_read_system_error(bno055_error_t *out, bno055_callback_t callback);

/**
 * Read the calibration status register of the sensor.
 * @param out out-parameter used for storing the result
 * @param callback callback that is called once the transaction is complete and "out" got set
 */
void bno055_read_calib_status(bno055_calib_status_t *out, bno055_callback_t callback);

/**
 * Write to the operation mode register of the sensor.
 * @param op_mode the operation mode to write
 * @param callback callback that is called once the transaction is complete
 */
void bno055_write_opr_mode(bno055_opr_mode_t op_mode, bno055_callback_t callback);

/**
 * Set the unit selection register of the sensor.
 * @param acc_unit the unit used for acceleration measurements
 * @param angular_rate_unit the unit used for angular rates
 * @param euler_angles_unit the unit used for euler angles
 * @param temperature_unit the unit used for temperatures
 * @param orientation_def the axis-frame used for the measurements
 * @param callback callback that is called once the transaction is complete
 */
void bno055_write_unit_selection(bno055_unit_sel_acc acc_unit, bno055_unit_sel_angular_rate angular_rate_unit,
                                 bno055_unit_sel_euler_angles euler_angles_unit,
                                 bno055_unit_sel_temperature temperature_unit,
                                 bno055_unit_sel_orientation_def orientation_def, bno055_callback_t callback);

/**
 * Set the axis remap register of the sensor.
 * @param new_x the axis that should be the new x-axis
 * @param new_y the axis that should be the new y-axis
 * @param new_z the axis that should be the new z-axis
 * @param callback callback that is called once the transaction is complete
 */
void bno055_write_remap_axis(bno055_axis_remap_axis_t new_x, bno055_axis_remap_axis_t new_y,
                             bno055_axis_remap_axis_t new_z, bno055_callback_t callback);

/**
 * Set the axis remap sign register
 * @param new_x_sign the sign of the x-axis
 * @param new_y_sign the sign of the y-axis
 * @param new_z_sign the sign of the z-axis
 * @param callback the callback that is called once the transaction is complete
 */
void bno055_write_remap_axis_sign(bno055_axis_remap_sign_t new_x_sign, bno055_axis_remap_sign_t new_y_sign,
                                  bno055_axis_remap_sign_t new_z_sign, bno055_callback_t callback);

/**
 * Read the result of the self test
 * @param out out-parameter used for storing the result
 * @param callback callback that is called once the transaction is complete and "out" got set
 */
void bno055_read_self_test(bno055_self_test_result_t *out, bno055_callback_t callback);

/**
 * Read the acceleration for all three axis multiplied with 100.
 * @param out out-parameter used for storing the result, needs to be sufficiently large to store all three values-
 * @param callback callback that is called once the transaction is complete and "out" got set
 */
void bno055_read_acc_xyz_mul_100(int16_t *out, bno055_callback_t callback);

/**
 * Read the gyroscopic angular rate for all three axis multiplied with 16.
 * @param out out-parameter used for storing the result, needs to be sufficiently large to store all three values-
 * @param callback callback that is called once the transaction is complete and "out" got set
 */
void bno055_read_gyr_xyz_mul_16(int16_t *out, bno055_callback_t callback);

/**
 * Read the euler-angles for all three axis multiplied with 16.
 * @param out out-parameter used for storing the result, needs to be sufficiently large to store all three values-
 * @param callback callback that is called once the transaction is complete and "out" got set
 */
void bno055_read_eul_xyz_2_mul_16(int16_t *out, bno055_callback_t callback);

#endif // FLIGHTCONTROLLER_BNO055_H
