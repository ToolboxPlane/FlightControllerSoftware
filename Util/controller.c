#include <math.h>
#include "controller.h"

controller_t roll_controller, pitch_controller;

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

void controller_init() {
    roll_controller.P = 12.0;
    pitch_controller.P = 20.0;

    pitch_controller.I = 0.0;
    roll_controller.I = 0.0;

    roll_controller.D = 0.0;
    pitch_controller.D = 0.0;

    roll_controller.i_area = pitch_controller.i_area = 0;
    roll_controller.delta_t = pitch_controller.delta_t = 0;
}

void controller_update(const state_t *state, const setpoint_t *setpoint,
                       out_state_t *out_state) {
    roll_controller.is_value = state->roll; // Remapping because of bno...
    pitch_controller.is_value = state->pitch;

    roll_controller.deriv = (int16_t)0; //@TODO
    pitch_controller.deriv = (int16_t)0;

    roll_controller.target_value = setpoint->roll;
    pitch_controller.target_value = setpoint->pitch;

    float roll_ctrl_val = update_controller(&roll_controller);
    float pitch_ctrl_val = update_controller(&pitch_controller);

    if(roll_ctrl_val < -500) {
        roll_ctrl_val = -500;
    } else if(roll_ctrl_val > 500) {
        roll_ctrl_val = 500;
    }

    if(pitch_ctrl_val < -500) {
        pitch_ctrl_val = -500;
    } else if(pitch_ctrl_val > 500) {
        pitch_ctrl_val = 500;
    }

    out_state->vtail_l = (int16_t)(-pitch_ctrl_val*sinf((roll_controller.is_value-45)/180.0f*M_PI));
    out_state->vtail_r = (int16_t)(-pitch_ctrl_val*sinf((roll_controller.is_value+45)/180.0f*M_PI));
    out_state->aileron_l = (int16_t)(-roll_ctrl_val);
    out_state->aileron_r = (int16_t)(-roll_ctrl_val);
}
