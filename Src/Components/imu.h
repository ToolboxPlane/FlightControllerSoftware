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

/**
 * Set of measurements from
 */
typedef struct {
    int16_t heading_mul_16; ///< Heading in euler angles, in degree, multiplied by 16
    int16_t pitch_mul_16;   ///< Pitch in euler angles, in degree, multiplied by 16
    int16_t roll_mul_16;    ///< Roll in euler angles, in degree, multiplied by 16

    int16_t d_heading_mul_16; ///< Heading angular rate in euler angles, in degree/second, multiplied by 16
    int16_t d_pitch_mul_16;   ///< Pitch angular rate in euler angles, in degree/second, multiplied by 16
    int16_t d_roll_mul_16;    ///< Roll angular rate in euler angles, in degree/second, multiplied by 16

    int16_t acc_x_mul_100; ///< Acceleration along x-axis, in m/s^2, multiplied by 100
    int16_t acc_y_mul_100; ///< Acceleration along y-axis, in m/s^2, multiplied by 100
    int16_t acc_z_mul_100; ///< Acceleration along z-axis, in m/s^2, multiplied by 100

    /**
     * Signals if the received data is fully usable, i.e.:
     *  * the system status is ok
     *  * all sensors are sufficiently calibrated
     */
    bool imu_ok;
} imu_data_t;

/**
 * Errors reported by the imu to the error handler component.
 */
typedef enum {
    IMU_ERROR_INIT_TIMEOUT = 1,
    IMU_ERROR_INIT_SELF_TEST = 2,
    IMU_ERROR_STATUS = 3,
    IMU_ERROR_READ_TIMEOUT = 4
} imu_error_t;

/**
 * @brief Initialize the IMU component.
 *
 * The initialization consists of the following task:
 *  * Call ::bno055_init to initialize the connection to the sensor
 *  * Set the chip to config (::bno055_write_opr_mode)
 *  * Wait for 20ms for a response:
 *      * if the chip did not respond call error_handle_error(IMU, IMU_ERROR_INIT_TIMEOUT)
 *      * if the response is not write-success call error_handle_error(BNO, response)
 *  * Run the self tests (::bno055_read_self_test)
 *  * Wait for 20ms for a response:
 *      * if the chip did not respond call error_handle_error(IMU, IMU_ERROR_INIT_TIMEOUT)
 *      * if the response is not read-success call error_handle_error(BNO, response)
 *  * check that [acc, gyr, mag, mcu]_passed are all set, if not call error_handle_error(IMU, IMU_ERROR_INIT_SELF_TEST)
 *  * Configure the units (::bno055_write_unit_selection) to the following units:
 *      * Acceleration to m/s^2
 *      * Angular velocity to deg/s
 *      * Angles to deg
 *      * Temperature to celsius
 *      * Orientation frame to windows
 *  * Wait for 20ms for a response:
 *      * if the chip did not respond call error_handle_error(IMU, IMU_ERROR_INIT_TIMEOUT)
 *      * if the response is not write-success call error_handle_error(BNO, response)
 *  * Configure the axis (::bno055_write_remap_axis) with y as the new x-axis, x as the new y-axis and z as the  z-axis.
 *  * Wait for 20ms for a response:
 *      * if the chip did not respond call error_handle_error(IMU, IMU_ERROR_INIT_TIMEOUT)
 *      * if the response is not write-success call error_handle_error(BNO, response)
 *  * Configure the axis sign (::bno055_write_remap_axis_sign) with x and y as positive, and z as negative sign
 *  * Wait for 20ms for a response:
 *      * if the chip did not respond call error_handle_error(IMU, IMU_ERROR_INIT_TIMEOUT)
 *      * if the response is not write-success call error_handle_error(BNO, response)
 *  * Set the chip to 9 degrees of freedom - fast magnetic calibration off mode (NDOF-FMC-OFF) (::bno055_write_opr_mode)
 *  * Wait for 20ms for a response:
 *      * if the chip did not respond call error_handle_error(IMU, IMU_ERROR_INIT_TIMEOUT)
 *      * if the response is not write-success call error_handle_error(BNO, response)
 */
void imu_init(void);

/**
 * @brief Parse result and start next sampling process
 *
 * If the last sampling process is not completed the function shall call
 * error_handler_handle_warning(ERROR_HANDLER_GROUP_IMU, IMU_ERROR_READ_TIMEOUT).
 *
 * Otherwise the following steps shall be performed:
 *  * If the last response was read-success:
 *      * If the last datum was Status set imu_ok to false if the mode is not "Sensor Fusion Algorithm Running"
 *      * If the last datum was Calib-Status:
 *          * set imu_ok to false if any of the fields is less than the respective threshold
 *          * make the result available (such that ::imu_get_latest_data returns the data and ::imu_data_available
 *            returns true)
 *      * Read the next datum, the (cyclic-) order is:
 *           1. Euler angles (::bno055_read_eul_xyz_2_mul_16)
 *           2. Gyroscopic angular rate (::bno055_read_gyr_xyz_mul_16)
 *           3. Acceleration (::bno055_read_acc_xyz_mul_100)
 *           4. System State (::bno055_read_system_status)
 *           5. Calibration status (::bno055_read_calib_status)
 *  * Otherwise:
 *      * Call error_handler_handle_warning(ERROR_HANDLER_GROUP_BNO055, response + 1)
 *      * Re-read the current datum
 */
void imu_start_sampling(void);

/**
 * @brief Get the last, fully received, set of measurements.
 *
 * @return a struct containing all measurement data.
 */
imu_data_t imu_get_latest_data(void);

/**
 * @brief Checks whether a new set of measurements was collected since the last call to get_latest_data().
 * @return true if new data is available.
 */
bool imu_data_available(void);

#endif // FLIGHTCONTROLLER_IMU_H
