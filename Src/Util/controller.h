//
// Created by paul on 29.12.17.
//

#ifndef FLIGHTCONTROLLER_CONTROLLER_H
#define FLIGHTCONTROLLER_CONTROLLER_H

#include <stdbool.h>

#include "../Components/imu.h"
#include "../out_state.h"
#include "../setpoint.h"

typedef struct controller {
    int16_t target_value, is_value;

    // Value = Value_num/Value_denom
    uint8_t P_num, P_denom;
    uint8_t I_num, I_denom;
    uint8_t D_num, D_denom;

    int16_t i_area, last_is_value;
    int16_t deriv;
    uint16_t delta_t;
} controller_t;

void controller_init(uint16_t delta_t);
void controller_update(volatile const imu_data_t *state, volatile const setpoint_t *setpoint,
                       volatile out_state_t *out_state);

#endif // FLIGHTCONTROLLER_CONTROLLER_H
