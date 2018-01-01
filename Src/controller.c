#include "controller.h"

float update_controller(controller_t* c) {
    float p_val = (c->target_value - c->is_value)*c->P;
    c->i_area += c->delta_t * c->is_value;
    if(c->last_is_value * c->is_value <= 0) {
        c->i_area = 0;
    }
    c->last_is_value = c->is_value;
    float i_val = c->i_area * c->I;
    float d_val = c->deriv * c->D;

    return p_val + i_val + d_val;
}
