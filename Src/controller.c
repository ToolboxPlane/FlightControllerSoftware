#include "controller.h"

controller_t roll_controller, pitch_controller, heading_controller;

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

void init_all_controller() {
    roll_controller.P = 1;
    pitch_controller.P = 1;
    heading_controller.P = 1;

    roll_controller.I = pitch_controller.I = heading_controller.I = 0.001;
    roll_controller.D = pitch_controller.D = heading_controller.D = 0.001;
}

void update_all_controller() {
    roll_controller.is_value = BNO055_ROLL;
    pitch_controller.is_value = BNO055_PITCH;
    heading_controller.is_value = BNO055_HEADING_PM;

    roll_controller.deriv = BNO055_GYRO_Z;
    heading_controller.deriv = BNO055_GYRO_X;
    pitch_controller.deriv = BNO055_GYRO_Y;

    float roll_ctrl_val = update_controller(&roll_controller);
    float heading_ctrl_val = update_controller(&heading_controller);
    float pitch_ctrl_val = update_controller(&pitch_controller);
}
