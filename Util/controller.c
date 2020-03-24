#include "math.h"
#include "controller.h"

controller_t roll_controller, pitch_controller;

int16_t update_controller(controller_t* c) {
    int16_t p_val = (int16_t)((c->target_value - c->is_value)*c->P);
    c->i_area += c->delta_t * c->is_value;
    if(c->last_is_value * c->is_value <= 0) {
        c->i_area = 0;
    }
    c->last_is_value = c->is_value;
    int16_t i_val = (int16_t)(c->i_area * c->I);
    int16_t d_val = (int16_t)(c->deriv * c->D);

    return p_val + i_val + d_val;
}

void controller_init(uint16_t delta_t) {
    roll_controller.P = 12.0;
    pitch_controller.P = 20.0;

    pitch_controller.I = 0.0;
    roll_controller.I = 0.0;

    roll_controller.D = 0.0;
    pitch_controller.D = 0.0;

    roll_controller.i_area = pitch_controller.i_area = 0;
    roll_controller.delta_t = pitch_controller.delta_t = delta_t;
}

void controller_update(const state_t *state, const setpoint_t *setpoint,
                       out_state_t *out_state) {
    roll_controller.is_value = state->roll / 2;
    pitch_controller.is_value = state->pitch / 2;

    roll_controller.deriv = state->roll_deriv;
    pitch_controller.deriv = state->pitch_deriv;

    roll_controller.target_value = setpoint->roll;
    pitch_controller.target_value = setpoint->pitch;

    int16_t roll_ctrl_val = update_controller(&roll_controller);
    int16_t pitch_ctrl_val = update_controller(&pitch_controller);

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

    out_state->elevon_l = (int16_t)(pitch_ctrl_val - roll_ctrl_val);
    out_state->elevon_r = (int16_t)(pitch_ctrl_val + roll_ctrl_val);
}
