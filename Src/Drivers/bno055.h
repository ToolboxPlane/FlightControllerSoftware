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
    BNO055_STATUS_SYSTEM_IDLE = 0x0,
    BNO055_STATUS_SYSTEM_ERROR = 0x1,
    BNO055_STATUS_INITIALIZING_PERIPHERALS = 0x2,
    BNO055_STATUS_SYSTEM_INITIALIZATION = 0x3,
    BNO055_STATUS_EXECUTING_SELFTEST = 0x4,
    BNO055_STATUS_SENSOR_FUSION_ALGORITHM_RUNNING = 0x5,
    BNO055_STATUS_SYSTEM_RUNNING_WITHOUT_FUSION_ALGORITHM = 0x6
} bno055_status_t;

/**
 * Possible values of the error register.
 */
typedef enum {
    BNO055_ERROR_NO_ERROR = 0x0,
    BNO055_ERROR_PERIPHERAL_INITIALIZATION_ERROR = 0x1,
    BNO055_ERROR_SYSTEM_INITIALIZATION_ERROR = 0x2,
    BNO055_ERROR_SELF_TEST_RESULT_FAILED = 0x3,
    BNO055_ERROR_REGISTER_MAP_VALUE_OUT_OF_RANGE = 0x4,
    BNO055_ERROR_REGISTER_MAP_ADDRESS_OUT_OF_RANGE = 0x5,
    BNO055_ERROR_REGISTER_MAP_WRITE_ERROR = 0x6,
    BNO055_ERROR_BNO_LOW_POWER_MODE_NOT_AVAILABLE_FOR_SELECTED_OPERATION_MODE = 0x7,
    BNO055_ERROR_ACCELEROMETER_POWER_MODE_NOT_AVAILABLE = 0x8,
    BNO055_ERROR_FUSION_ALGORITHM_CONFIGURATION_ERROR = 0x9,
    BNO055_ERROR_SENSOR_CONFIGURATION_ERROR = 0xa
} bno055_error_t;

/**
 * Possible operation modes of the sensor.
 */
typedef enum {
    BNO055_OPR_MODE_CONFIG_MODE = 0b0000,
    BNO055_OPR_MODE_ACC_ONLY = 0b0001,
    BNO055_OPR_MODE_MAG_ONLY = 0b0010,
    BNO055_OPR_MODE_GYRO_ONLY = 0b0011,
    BNO055_OPR_MODE_ACC_MAG = 0b0100,
    BNO055_OPR_MODE_ACC_GYRO = 0b0101,
    BNO055_OPR_MODE_MAG_GYRO = 0b0110,
    BNO055_OPR_MODE_AMG = 0b0111,
    BNO055_OPR_MODE_IMU = 0b1000,
    BNO055_OPR_MODE_COMPASS = 0b1001,
    BNO055_OPR_MODE_M4G = 0b1010,
    BNO055_OPR_MODE_NDOF_FMC_OFF = 0b1011,
    BNO055_OPR_MODE_NDOF = 0b1100
} bno055_opr_mode_t;

/**
 * Possible axis names used for remapping.
 */
typedef enum {
    BNO055_AXIS_REMAP_X_AXIS = 0,
    BNO055_AXIS_REMAP_Y_AXIS = 1,
    BNO055_AXIS_REMAP_Z_AXIS = 2
} bno055_axis_remap_axis_t;

/**
 * Possible axis signs used for sign remapping.
 */
typedef enum { BNO055_AXIS_REMAP_SIGN_POSITIVE = 0, BNO055_AXIS_REMAP_SIGN_NEGATIVE = 1 } bno055_axis_remap_sign_t;

/**
 * Possible acceleration units for the unit selection.
 */
typedef enum { BNO055_UNIT_SEL_ACC_MPS2 = 0, BNO055_UNIT_SEL_ACC_MG = 1 } bno055_unit_sel_acc;

/**
 * Possible angular rate units for the unit selection.
 */
typedef enum {
    BNO055_UNIT_SEL_ANGULAR_RATE_DPS = 0,
    BNO055_UNIT_SEL_ANGULAR_RATE_RPS = 1
} bno055_unit_sel_angular_rate;

/**
 * Possible euler angle units for the unit selection.
 */
typedef enum {
    BNO055_UNIT_SEL_EULER_ANGLES_DEGREES = 0,
    BNO055_UNIT_SEL_EULER_ANGLES_RADIANS = 1
} bno055_unit_sel_euler_angles;

/**
 * Possible temperature units for the unit selection.
 */
typedef enum {
    BNO055_UNIT_SEL_TEMPERATURE_CELSIUS = 0,
    BNO055_UNIT_SEL_TEMPERATURE_FAHRENHEIT = 1
} bno055_unit_sel_temperature;

/**
 * Possible axis-frame options for the unit selection.
 */
typedef enum {
    BNO055_UNIT_SEL_ORIENTATION_DEF_WINDOWS = 0,
    BNO055_UNIT_SEL_ORIENTATION_DEF_ANDROID = 1
} bno055_unit_sel_orientation_def;

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
 * @brief Initialize the physical connection to the BNO, this will not initialize the sensor itself.
 *
 * The initialization consists of the following tasks:
 *  * call ::bno055_uart_init
 */
void bno055_init(void);

/**
 * @brief Reset the sensor by writing a 1 to the respective bit in the system trigger register.
 *
 * This function calls ::bno055_uart_write_register with the following arguments:
 *  * reg: SYS_TRIGGER (0x3F)
 *  * buf/len: one byte, containing one bit at the 5th position
 *  * callback: the callback passed to this function
 *
 * @param callback function to be called when the chip sends a response
 */
void bno055_write_reset(bno055_callback_t callback);

/**
 * @brief Read the system status register of the sensor.
 *
 * This function calls ::bno055_uart_read_register with the following arguments:
 *  * reg: SYS_STATUS (0x39)
 *  * len: one byte
 *  * callback: the callback passed to this function
 *  * out: the out pointer, as passed to this function
 *
 * @param out out-parameter used for storing the result
 * @param callback callback that is called once the transaction is complete and "out" got set
 */
void bno055_read_system_status(bno055_status_t *out, bno055_callback_t callback);

/**
 * @brief Read the system error register of the sensor.
 *
 * This function calls ::bno055_uart_read_register with the following arguments:
 *  * reg: SYS_ERROR (0x3A)
 *  * len: one byte
 *  * callback: the callback passed to this function
 *  * out: the out pointer, as passed to this function
 *
 * @param out out-parameter used for storing the result
 * @param callback callback that is called once the transaction is complete and "out" got set
 */
void bno055_read_system_error(bno055_error_t *out, bno055_callback_t callback);

/**
 * @brief Read the calibration status register of the sensor.
 *
 * This function calls ::bno055_uart_read_register with the following arguments:
 *  * reg: CALIB_STAT (0x35)
 *  * len: one byte
 *  * callback: the callback passed to this function
 *  * out: the out pointer, as passed to this function
 *
 * @param out out-parameter used for storing the result
 * @param callback callback that is called once the transaction is complete and "out" got set
 */
void bno055_read_calib_status(bno055_calib_status_t *out, bno055_callback_t callback);

/**
 * @brief Write to the operation mode register of the sensor.
 *
 * This function calls ::bno055_uart_write_register with the following arguments:
 *  * reg: OPR_MODE (0x3D)
 *  * buf/len: one byte, containing the numerical representation of the operation mode
 *  * callback: the callback passed to this function
 *
 * @param op_mode the operation mode to write
 * @param callback callback that is called once the transaction is complete
 */
void bno055_write_opr_mode(bno055_opr_mode_t op_mode, bno055_callback_t callback);

/**
 * @brief Set the unit selection register of the sensor.
 *
 * This function calls ::bno055_uart_write_register with the following arguments:
 *  * reg: UNIT_SEL (0x3B)
 *  * buf/len: one byte, with the following bits:
 *      * 0: Acceleration unit (0: m/s^2, 1: mg)
 *      * 1: Gyro unit (0: dps, 1: rps)
 *      * 2: Euler-Angle unit (0: degrees, 1: radians)
 *      * 3: Temperature unit (0: celsius, 1: fahrenheit)
 *      * 7: Orientation mode (0: windows, 1: android)
 *  * callback: the callback passed to this function
 *
 * @warning The Datasheet (Revision 1.8) is inconsistent with regards to the bit-index between different tables, these
 * offsets have been tested manually.
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
 * @brief Set the axis remap register of the sensor.
 *
 * This function calls ::bno055_uart_write_register with the following arguments:
 *  * reg: AXIS_MAP_CONFIG (0x41)
 *  * buf/len: one byte, with the following bits:
 *      * 0,1: new x-axis
 *      * 2,3: new y-axis
 *      * 4,5: new z-axis
 *  * callback: the callback passed to this function
 *
 * With the axis represented as: (0: x, 1: y, 2: z)
 *
 * @param new_x the current axis that should be the new x-axis
 * @param new_y the current axis that should be the new y-axis
 * @param new_z the current axis that should be the new z-axis
 * @param callback callback that is called once the transaction is complete
 */
void bno055_write_remap_axis(bno055_axis_remap_axis_t new_x, bno055_axis_remap_axis_t new_y,
                             bno055_axis_remap_axis_t new_z, bno055_callback_t callback);

/**
 * @brief Set the axis remap sign register.
 *
 * This function calls ::bno055_uart_write_register with the following arguments:
 *  * reg: AXIS_MAP_SIGN (0x42)
 *  * buf/len: one byte, with the following bits:
 *      * 0: Z-Sign (0 positive, 1 negative)
 *      * 1: Y-Sign (0 positive, 1 negative)
 *      * 2: X-Sign (0 positive, 1 negative)
 *  * callback: the callback passed to this function
 *
 * @param new_x_sign the sign of the x-axis
 * @param new_y_sign the sign of the y-axis
 * @param new_z_sign the sign of the z-axis
 * @param callback the callback that is called once the transaction is complete
 */
void bno055_write_remap_axis_sign(bno055_axis_remap_sign_t new_x_sign, bno055_axis_remap_sign_t new_y_sign,
                                  bno055_axis_remap_sign_t new_z_sign, bno055_callback_t callback);

/**
 * @brief Read the result of the self test.
 *
 * This function calls ::bno055_uart_read_register with the following arguments:
 *  * reg: ST_RESULT (0x36)
 *  * len: one byte
 *  * callback: the callback passed to this function
 *  * out: the out pointer, as passed to this function
 *
 * @param out out-parameter used for storing the result
 * @param callback callback that is called once the transaction is complete and "out" got set
 */
void bno055_read_self_test(bno055_self_test_result_t *out, bno055_callback_t callback);

/**
 * @brief Read the acceleration for all three axis multiplied with 100.
 *
 * This function calls ::bno055_uart_read_register with the following arguments:
 *  * reg: LIA_DATA_X_LSB (0x28)
 *  * len: 6 (three words of two bytes each)
 *  * callback: the callback passed to this function
 *  * out: the out pointer, as passed to this function
 *
 * @param out out-parameter used for storing the result, needs to be sufficiently large to store all three values-
 * @param callback callback that is called once the transaction is complete and "out" got set
 */
void bno055_read_acc_xyz_mul_100(int16_t *out, bno055_callback_t callback);

/**
 * @brief Read the gyroscopic angular rate for all three axis multiplied with 16.
 *
 * This function calls ::bno055_uart_read_register with the following arguments:
 *  * reg: GYR_DATA_X_LSB (0x14)
 *  * len: 6 (three words of two bytes each)
 *  * callback: the callback passed to this function
 *  * out: the out pointer, as passed to this function
 *
 * @param out out-parameter used for storing the result, needs to be sufficiently large to store all three values-
 * @param callback callback that is called once the transaction is complete and "out" got set
 */
void bno055_read_gyr_xyz_mul_16(int16_t *out, bno055_callback_t callback);

/**
 * @brief Read the euler-angles for all three axis multiplied with 16.
 *
 * This function calls ::bno055_uart_read_register with the following arguments:
 *  * reg: EUL_DATA_X_LSB (0x1A)
 *  * len: 6 (three words of two bytes each)
 *  * callback: the callback passed to this function
 *  * out: the out pointer, as passed to this function
 *
 * @param out out-parameter used for storing the result, needs to be sufficiently large to store all three values-
 * @param callback callback that is called once the transaction is complete and "out" got set
 */
void bno055_read_eul_xyz_2_mul_16(int16_t *out, bno055_callback_t callback);

#endif // FLIGHTCONTROLLER_BNO055_H
