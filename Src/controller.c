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

void init_all_controller() {
    roll_controller.P = 10;
    pitch_controller.P = 10;

    roll_controller.I = pitch_controller.I = 0.0;
    roll_controller.D = pitch_controller.D = 0.0;

    roll_controller.i_area = pitch_controller.i_area = 0;
}

void update_all_controller() {
    roll_controller.is_value = (uint16_t)(BNO055_ROLL+180) - 180.0f;
    pitch_controller.is_value = (uint16_t)(BNO055_PITCH+180) - 180.0f;

    roll_controller.deriv = (int16_t)BNO055_GYRO_Z;
    pitch_controller.deriv = (int16_t)BNO055_GYRO_Y;

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

    servoPosition[VTAIL_L] = servoPosition[VTAIL_R] = (int16_t)(pitch_ctrl_val);
    servoPosition[AILERON_L] = servoPosition[AILERON_R] = (int16_t)(roll_ctrl_val);
}
