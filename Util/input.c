/**
 * @file input.c
 * @author paul
 * @date 18.04.19
 * @brief input @TODO
 */

#include "input.h"
#include "../Drivers/bno055_uart.h"

void input_init(void) {
    bno055_init();
}

state_t input_get_state(void) {
    state_t state;
    state.roll = bno055_eul_y_2();
    state.pitch = -bno055_eul_z_2();
    state.heading = bno055_eul_x_2();
    if (state.heading > 180 * 2) {
        state.heading -= 360 * 2;
    }
    state.roll_deriv = -bno055_gyr_y();
    state.pitch_deriv = bno055_gyr_z();
    state.heading_deriv = bno055_gyr_x();
    state.acc_x = bno055_acc_y();
    state.acc_y = bno055_acc_x();
    state.acc_z = bno055_acc_z();

    state.bno_state = bno055_status();
    if (state.bno_state == system_error) {
        state.error = bno055_error();
    } else {
        state.error = 0;
    }
    state.calib_stat =bno055_calib_stat();

    return state;
}
