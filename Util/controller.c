#include "math.h"
#include "controller.h"

static controller_t roll_controller, pitch_controller;

#define CLAMP(x, min, max) ((x) < (min) ? (min) : ((x) > (max) ? (max) : (x)))

int16_t fix_angle(int16_t angle) {
    while (angle < -180) {
        angle += 360;
    }
    while (angle > 180) {
        angle -= 360;
    }
    return angle;
}

int16_t update_controller(controller_t *c) {
    int16_t error = fix_angle(c->target_value - c->is_value);
    int16_t p_val = (int16_t)(error * c->P);
    c->i_area += c->delta_t * error;
    if(c->last_is_value * c->is_value <= 0) {
        c->i_area = 0;
    }
    c->last_is_value = c->is_value;
    int16_t i_val = (int16_t)(c->i_area * c->I);
    int16_t d_val = (int16_t)(c->deriv * c->D);

    return p_val + i_val + d_val;
}

void controller_init(uint16_t delta_t) {
    roll_controller.P = 12.0F;
    pitch_controller.P = 20.0F;

    pitch_controller.I = 0.0F;
    roll_controller.I = 0.0F;

    roll_controller.D = 0.0F;
    pitch_controller.D = 0.0F;

    roll_controller.i_area = pitch_controller.i_area = 0;
    roll_controller.delta_t = pitch_controller.delta_t = delta_t;
}

void controller_update(volatile const state_t *state, volatile const setpoint_t *setpoint,
                       volatile out_state_t *out_state) {
    roll_controller.is_value = state->roll / 2;
    pitch_controller.is_value = state->pitch / 2;

    roll_controller.deriv = state->roll_deriv;
    pitch_controller.deriv = state->pitch_deriv;

    roll_controller.target_value = setpoint->roll;
    pitch_controller.target_value = setpoint->pitch;

    int16_t roll_ctrl_val = update_controller(&roll_controller);
    int16_t pitch_ctrl_val = (int16_t)(update_controller(&pitch_controller) * cosinus(state->roll/2));

    out_state->elevon_r = CLAMP((int16_t)(-pitch_ctrl_val + roll_ctrl_val), -500, 500);
    out_state->elevon_l = CLAMP((int16_t)(pitch_ctrl_val + roll_ctrl_val), -500, 500);
}
