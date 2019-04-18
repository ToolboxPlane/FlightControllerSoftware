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

void init_all_controller(void);
void update_all_controller(const state_t *state, const setpoint_t *setpoint,
        out_state_t *out_state);

#endif //FLIGHTCONTROLLER_CONTROLLER_H
