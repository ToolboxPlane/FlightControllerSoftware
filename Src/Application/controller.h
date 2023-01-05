/**
 * @file controller.h
 * @author Paul Nykiel
 * @date 27.11.22
 * @brief Declaration of the controller application.
 * @ingroup Application
 */
#ifndef FLIGHTCONTROLLER_CONTROLLER_H
#define FLIGHTCONTROLLER_CONTROLLER_H

#include "Components/imu.h"

/**
 * Result of the attitude controller.
 */
typedef struct {
    int16_t elevon_left;  ///< Desired command for the left elevon, in [-500, 500]
    int16_t elevon_right; ///< Desired command for the right elevon, in [-500, 500]
} controller_result_t;

/**
 * @brief Update the attitude P-controller.
 *
 * This function performs the following calculations for both the roll and pitch axis:
 *  * Calculate the error along the axis as the current value subtracted from the set point
 *  * Normalize the error to the range [-180, 180] to find the shortest rotation direction
 *  * Multiply the error with the respective P-constant to get a command along the axis.

 * The individual pseudo elevon commands are then calculated as:
 *  * elevon_left = - roll_cmd - pitch_cmd
 *  * elevon_right = roll_cmd - pitch_cmd
 *
 * The actual controller-result is then calculated by clamping the elevon signals to [-500, 500].
 *
 * A positive roll error, respective roll command shall result in the plane banking right;
 * a positive pitch error, respective pitch command shall result in the plane pitching down.
 * A positive elevon command shall result in the elevon deflecting upwards.
 *
 * @param imu_data the current state of the plane
 * @param roll_set_point the desired roll angle in degree
 * @param pitch_set_point the desired pitch angle in degree
 * @return the resulting elevon positions.
 */
controller_result_t controller_update(const imu_data_t *imu_data, int16_t roll_set_point, int16_t pitch_set_point);

#endif // FLIGHTCONTROLLER_CONTROLLER_H
