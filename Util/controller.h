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
    float target_value, is_value;

    float P,I,D;

    float i_area, last_is_value;
    float deriv;
    float delta_t;
} controller_t;

void controller_init(void);
void controller_update(const state_t *state, const setpoint_t *setpoint,
                       out_state_t *out_state);

#endif //FLIGHTCONTROLLER_CONTROLLER_H
