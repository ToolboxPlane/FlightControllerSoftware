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
    /**
     * Orientation in euler angles (degree) multiplied by 16
     */
    int16_t heading_mul_16, pitch_mul_16, roll_mul_16;

    /**
     * Angular rates in euler angles (degree/second) multiplied by 16
     */
    int16_t d_heading_mul_16, d_pitch_mul_16, d_roll_mul_16;

    /**
     * Acceleration in all three axis (m/s^2) multiplied by 100
     */
    int16_t acc_x_mul_100, acc_y_mul_100, acc_z_mul_100;

    /**
     * Signals if the received data is fully usable, i.e.:
     *  * the system status is ok
     *  * all sensors are sufficiently calibrated
     */
    bool imu_ok;
} imu_data_t;

typedef enum {
    IMU_ERROR_INIT_TIMEOUT = 1,
    IMU_ERROR_INIT_SELF_TEST = 2,
    IMU_ERROR_STATUS = 3,
} imu_error_t;

/**
 * Initialize the IMU component.
 * First the bno055 module is initialized and then the following operations are performed:
 *  * Set the chip to config mode
 *  * Run the self test (and check the result)
 *  * Configure the units
 *  * Remap the axis and corresponding signs
 *  * Set the sensor NDOF-FMC-OFF (9 degrees of freedom - fast magnetic calibration off)
 */
void imu_init(void);

/**
 * Start sampling of the sensor.
 *
 * This reads all data into an internal imu_data_t struct, if an error occurs sampling is aborted (and no new data
 * is reported).
 *
 * @warning Currently only one datum is read per call to this function, i.e. multiple calls are necessary to read
 * all data (see #41).
 */
void imu_start_sampling(void);

/**
 * Get the last, fully received, set of measurements.
 * @return a struct containing all measurement data.
 */
imu_data_t imu_get_latest_data(void);

/**
 * Checks whether a new set of measurements was collected since the last call to get_latest_data().
 * @return true if new data is available.
 */
bool imu_data_available(void);

#endif // FLIGHTCONTROLLER_IMU_H
