//
// Created by paul on 29.12.17.
//

#ifndef FLIGHTCONTROLLER_CONTROLLER_H
#define FLIGHTCONTROLLER_CONTROLLER_H

#include <stdbool.h>
#include "../state.h"
#include "../setpoint.h"
#include "../out_state.h"

typedef struct controller {
    int16_t target_value, is_value;

    float P,I,D;

    int16_t i_area, last_is_value;
    int16_t deriv;
    int16_t delta_t;
} controller_t;

void controller_init(uint16_t delta_t);
void controller_update(const state_t *state, const setpoint_t *setpoint,
                       out_state_t *out_state);

#endif //FLIGHTCONTROLLER_CONTROLLER_H
