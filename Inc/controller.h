//
// Created by paul on 29.12.17.
//

#ifndef FLIGHTCONTROLLER_CONTROLLER_H
#define FLIGHTCONTROLLER_CONTROLLER_H

#include <stdbool.h>
#include "main.h"

typedef struct controller {
    float target_value, is_value;

    float P,I,D;

    float i_area, last_is_value;
    float deriv;
    float delta_t;
} controller_t;

float update_controller(controller_t* c);

void init_all_controller();
void update_all_controller();

extern controller_t roll_controller, pitch_controller;

#endif //FLIGHTCONTROLLER_CONTROLLER_H
