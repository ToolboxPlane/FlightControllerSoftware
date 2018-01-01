//
// Created by paul on 29.12.17.
//

#ifndef FLIGHTCONTROLLER_CONTROLLER_H
#define FLIGHTCONTROLLER_CONTROLLER_H

typedef struct controller {
    float target_value, is_value;

    float P,I,D;

    float i_area, last_is_value;
    float deriv;
    float delta_t;
} controller_t;

float update_controller(controller_t* c);

#endif //FLIGHTCONTROLLER_CONTROLLER_H
