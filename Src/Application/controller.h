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
    /**
     * Desired command for the actuators, in [-500, 500]
     */
    int16_t elevon_left, elevon_right;
} controller_result_t;

/**
 * Update the attitude P-controller.
 * @param imu_data the current state of the plane
 * @param roll_setpoint the desired roll angle in degree
 * @param pitch_setpoint the desired pitch angle in degree
 * @return the resulting elevon positions.
 */
controller_result_t controller_update(const imu_data_t *imu_data, int16_t roll_setpoint, int16_t pitch_setpoint);

#endif // FLIGHTCONTROLLER_CONTROLLER_H
