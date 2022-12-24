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

typedef struct {
    int16_t elevon_left, elevon_right;
} controller_result_t;

controller_result_t controller_update(const imu_data_t *imu_data, int16_t roll_setpoint, int16_t pitch_setpoint);

#endif // FLIGHTCONTROLLER_CONTROLLER_H
